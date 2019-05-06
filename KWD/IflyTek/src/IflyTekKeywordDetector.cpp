#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <unistd.h> //sleep func

// Iflytek msc header files.
#include <qivw.h>
#include <msp_cmn.h>
#include <msp_errors.h>

#include <Utils/Logging/Logger.h>
#include "IflyTekKeywordDetector.h"

/// String to identify log entries originating from this file.
static const std::string TAG("IflyTekKeywordDetector");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace kwd {

using namespace utils::sharedbuffer;

/// The string of appid from IflyTek register account.
static const std::string DEFAULT_LOGINPARAMS("appid = 5c3d4427,work_dir = .");

/// The string of paraments default ivw msc configure.
static const std::string DEFAULT_SSBPARAMS("ivw_threshold=0:1450,sst=wakeup,ivw_res_path =fo|res/ivw/wakeupresource.jet");

/// The number of hertz per kilohertz.
static const size_t HERTZ_PER_KILOHERTZ = 1000;

/// The Sensory compatible AVS sample rate of 16 kHz.
static const unsigned int SENSORY_COMPATIBLE_SAMPLE_RATE = 16000;

/// The timeout to use for read calls to the SharedDataStream.
const std::chrono::milliseconds TIMEOUT_FOR_READ_CALLS = std::chrono::milliseconds(1000);

std::unique_ptr<IflyTekKeywordDetector> IflyTekKeywordDetector::create(
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
	std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>> keywordObserver,
	std::chrono::milliseconds maxSamplesPerPush) {
	if(!stream) {
		AISDK_ERROR(LX("CreateFiled").d("reason", "nullStream"));
		return nullptr;
	}

	auto detector = std::unique_ptr<IflyTekKeywordDetector>(new IflyTekKeywordDetector(stream, keywordObserver, maxSamplesPerPush));
	if(!detector->init()) {
		AISDK_ERROR(LX("CreateFailed").d("reason", "initDetectorFailed"));
		return nullptr;
	}

	return detector;
}

IflyTekKeywordDetector::~IflyTekKeywordDetector() {
    m_isShuttingDown = true;
    if (m_detectionThread.joinable()) {
        m_detectionThread.join();
    }

	const char *reason = "Nothing-op";	
    QIVWSessionEnd(m_sessionId.c_str(), reason);
}

IflyTekKeywordDetector::IflyTekKeywordDetector(
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
	std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>> keywordObserver,
	std::chrono::milliseconds maxSamplesPerPush):
		GenericKeywordDetector(keywordObserver),
		m_stream{stream},
		m_maxSamplesPerPush((SENSORY_COMPATIBLE_SAMPLE_RATE/HERTZ_PER_KILOHERTZ) * maxSamplesPerPush.count()){
}

bool IflyTekKeywordDetector::init() {
	// Login msc server.
	auto errCode = MSPLogin(NULL, NULL, DEFAULT_LOGINPARAMS.c_str());
	if (MSP_SUCCESS != errCode) {
		AISDK_ERROR(LX("initFailed").d("reason", "loginFailed").d("code", errCode));
		return false;//登录失败，退出登录
	}

	// Don't need grammar for awake of ivw.
	m_sessionId = QIVWSessionBegin(NULL, DEFAULT_SSBPARAMS.c_str(), &errCode);
	if (errCode != MSP_SUCCESS){
		AISDK_ERROR(LX("initFailed").d("reason", "QIVWSessionBeginFailed").d("code", errCode));
		return false;
	}

	// Register wakeup event notify callback.
	errCode = QIVWRegisterNotify(m_sessionId.c_str(), keyWordDetectedCallback, reinterpret_cast<void *>(this));
	if (errCode != MSP_SUCCESS) {
		AISDK_ERROR(LX("initFailed").d("reason", "QIVWRegisterNotifyFailed").d("code", errCode));
		return false;
	}

	// Create a @c Reader's to read data from the @c SharedBufferStream.
    m_streamReader = m_stream->createReader(Reader::Policy::BLOCKING);
    if (!m_streamReader) {
        AISDK_ERROR(LX("initFailed").d("reason", "createStreamReaderFailed"));
        return false;
    }

	m_isShuttingDown = false;
	m_detectionThread = std::thread(&IflyTekKeywordDetector::detectionLoop, this);
	
	AISDK_INFO(LX("initSuccessed").d("newSessionId", m_sessionId));
	return true;
}

int IflyTekKeywordDetector::keyWordDetectedCallback(
	const char *sessionID, int msg, int param1, int param2,
	const void *info, void *userData) {
	AISDK_DEBUG5(LX("keyWordDetectedCallback")
				.d("sessionId", sessionID)
				.d("msg", msg)
				.d("param1", param1)
				.d("param2", param2)
				.d("info", static_cast<const char *>(info)));
				
	// Convert userData to @c IflyTekKeywordDetector static point.
	IflyTekKeywordDetector* engine = static_cast<IflyTekKeywordDetector*>(userData);
	if(MSP_IVW_MSG_ERROR == msg) { //唤醒出错消息
		AISDK_WARN(LX("keyWordDetectedCallback").d("reason", "MSP_IVW_MSG_ERROR"));
		return 0;
	} else if (MSP_IVW_MSG_WAKEUP == msg) { //唤醒成功消息
		AISDK_INFO(LX("keyWordDetectedCallback").d("reason", "MSP_IVW_MSG_WAKEUP").d("info", static_cast<const char *>(info)));
	}

	// Parser beginIndex and endIndex.
	/**
	 * below is info content:
	 * {"sst":"wakeup", "id":0, "score":2077, "bos":1070, "eos":2000 ,"keyword":"xiao3kang1xiao3kang1"}
	 * To-Do: bos and eos value.
	 */

	// Notify keyword observer if detecte keyword wakeup event successfuly. defaule set beginIndex and endIndex: 0.
	engine->notifyKeyWordObservers(engine->m_stream, "xiaokang", 0, 0);

	return 0;
}

void IflyTekKeywordDetector::detectionLoop() {
	std::vector<int16_t> audioDataToPush(m_maxSamplesPerPush); //160*2 = 320 = 20ms 
	int errCode = MSP_SUCCESS;
	int audioStatus = MSP_AUDIO_SAMPLE_FIRST;
	ssize_t wordsRead;
	std::ofstream output("/tmp/Backup.pcm");

	while(!m_isShuttingDown) {

		bool didErrorOccur = false;
		// Start read data.
		wordsRead = readFromStream(
				m_streamReader,
				m_stream,
				audioDataToPush.data(),
				audioDataToPush.size(),
				TIMEOUT_FOR_READ_CALLS,
				&didErrorOccur);
		// Error occurrence maybe reader close.
		if(didErrorOccur) {
			break;
		} else if (wordsRead == Reader::Error::OVERRUN) {
			audioStatus = MSP_AUDIO_SAMPLE_FIRST;
			
		} else if(wordsRead > 0) {
			void *pbuf8 = audioDataToPush.data();
			output.write(static_cast<char *>(pbuf8), wordsRead*sizeof(*audioDataToPush.data()));

			errCode = QIVWAudioWrite(
				m_sessionId.c_str(),
				audioDataToPush.data(),
				wordsRead*sizeof(*audioDataToPush.data()),
				audioStatus);
			if (MSP_SUCCESS != errCode){
				std::cout << __FILE__ << ":" << __LINE__ << " QIVWAudioWrite failed! error code: " << errCode << std::endl;
				
			}

			audioStatus = MSP_AUDIO_SAMPLE_CONTINUE;
		}
		
	}
	// Close the @c Reader which read stream occurence.
	m_streamReader->close();
}

}
}
