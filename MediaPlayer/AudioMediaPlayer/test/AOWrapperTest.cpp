/*
 * Copyright 2018 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <cstdio>

#include "Utils/Attachment/AttachmentReader.h"
#include "Utils/MediaPlayer/MediaPlayerObserverInterface.h"
#include "AudioMediaPlayer/AOWrapper.h"

using namespace aisdk::mediaPlayer::ffmpeg;

int flags = 0;
	
namespace util {
int kbhit();
}

int util::kbhit()
{
    struct timeval tv;
    fd_set rdfs;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);

    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);

    return FD_ISSET(STDIN_FILENO, &rdfs);
}
class MockAttachmentReader : public aisdk::utils::attachment::AttachmentReader {
public:
	MockAttachmentReader(std::shared_ptr<std::ifstream> stream):m_stream{stream} {

	};

	/// @name AttachmentReader method;
	/// @{
	std::size_t read(
        void* buf,
        std::size_t numBytes,
        ReadStatus* readStatus,
        std::chrono::milliseconds timeoutMs = std::chrono::milliseconds(0)) override;
	bool seek(uint64_t offset) override { return true; };
	uint64_t getNumUnreadBytes() override { return 0; };
	void close(ClosePoint closePoint = ClosePoint::AFTER_DRAINING_CURRENT_BUFFER) override;
	/// @}
	~MockAttachmentReader() {
		std::cout << "~MockAttachmentReader destory" << std::endl;
		close();
	}
private:
	std::shared_ptr<std::ifstream> m_stream;
};

std::size_t MockAttachmentReader::read(
	void* buf,
	std::size_t numBytes,
	ReadStatus* readStatus,
	std::chrono::milliseconds timeoutMs) {
	(*readStatus) = aisdk::utils::attachment::AttachmentReader::ReadStatus::OK;
    m_stream->read(reinterpret_cast<char*>(buf), numBytes);
    auto bytesRead = m_stream->gcount();
    if (!bytesRead) {
		(*readStatus) = aisdk::utils::attachment::AttachmentReader::ReadStatus::CLOSED;
        if (m_stream->bad()) {
			std::cout << "readFailed::occrous." << std::endl;
            return 0;
        }
		std::cout << "readFailed::bytes: " << bytesRead << std::endl;
        return 0;
    }
	
    return bytesRead;
}

void MockAttachmentReader::close(ClosePoint closePoint) {
	m_stream.reset();
}


class AudioPlayerTest 
	: public aisdk::utils::mediaPlayer::MediaPlayerObserverInterface
	, public std::enable_shared_from_this<AudioPlayerTest> {
public:
	/// Create a object @c AudioPlayerTest
	static std::shared_ptr<AudioPlayerTest> createNew();

	/// Destructs.
	~AudioPlayerTest(){ 
		std::cout << "AudioPlayerTest destructor..." << std::endl;
		if(m_playWrapper) 
			m_playWrapper.reset();
	}

	/// Set Observer
	bool init();

	bool run(std::shared_ptr<aisdk::utils::attachment::AttachmentReader> attachmentReader);

	bool run(std::shared_ptr<std::istream> stream);

	bool run(std::string &url, std::chrono::milliseconds &offset);

	/// Master process tasks
	bool play();
	
	/// @name MediaPlayerObserverInterface functions
	/// @{
	void onPlaybackStarted(SourceId id) override;

    void onPlaybackFinished(SourceId id) override;

    void onPlaybackError(SourceId id, const aisdk::utils::mediaPlayer::ErrorType& type, std::string error) override;

    void onPlaybackStopped(SourceId id) override;

	void onPlaybackPaused(SourceId id) override;

	void onPlaybackResumed(SourceId id) override;

	///@}
	
private:
	std::shared_ptr<AOWrapper> m_playWrapper;

	std::shared_ptr<AOWrapper> m_playStreamWrapper;

	AOWrapper::SourceId m_sourceID;

	AOWrapper::SourceId m_sourceStreamID;

	std::shared_ptr<std::istream> m_stream;
};

std::shared_ptr<AudioPlayerTest> AudioPlayerTest::createNew() {
	std::cout << "CreateNew entry" << std::endl;
	auto instance = std::shared_ptr<AudioPlayerTest>(new AudioPlayerTest);
	if(instance->init()) {
		std::cout << "created:reason=initializedSuccess" << std::endl;
		return instance;
	}

	return nullptr;
}

bool AudioPlayerTest::init(){
	/// Create object for the @c AOEngine.
	auto engine = AOEngine::create();
	if(!engine) {
		std::cout << "createFailed:reason=createEngineFailed" << std::endl;
		return false;
	}
	
	/// Create PaWrapper object
	m_playWrapper = AOWrapper::create(engine);
	if(!m_playWrapper){
		std::cout << "CreateFailed:reason=createWrapperFailed" << std::endl;
		return false;
	}

	if(flags) {
		m_playStreamWrapper = AOWrapper::create(engine);
		if(!m_playStreamWrapper){
			std::cout << "CreateFailed:reason=createWrapperFailed" << std::endl;
			return false;
		}
		m_playStreamWrapper->setObserver(shared_from_this());
	}
	
	m_playWrapper->setObserver(shared_from_this());
	
	return true;
}


bool AudioPlayerTest::run(std::shared_ptr<aisdk::utils::attachment::AttachmentReader> attachmentReader) {
#if 1
	aisdk::utils::AudioFormat format{.encoding = aisdk::utils::AudioFormat::Encoding::LPCM,
                       .endianness = aisdk::utils::AudioFormat::Endianness::LITTLE,
                       .sampleRateHz = 16000,
                       .sampleSizeInBits = 16,
                       .numChannels = 1,
                       .dataSigned = true};
	#endif
	//aisdk::utils::AudioFormat format;
	m_sourceID = m_playWrapper->setSource(attachmentReader, &format);
	if(aisdk::utils::mediaPlayer::MediaPlayerInterface::ERROR == m_sourceID){
		std::cout << "run:reason=setSourceStreamFailed" << std::endl;
		return false;
	}
	
	std::cout << "newSourceId= " << m_sourceID << std::endl;

	this->play();

	return true;

}

bool AudioPlayerTest::run(std::shared_ptr<std::istream> stream){
	
	m_sourceID = m_playWrapper->setSource(stream, true);
	if(aisdk::utils::mediaPlayer::MediaPlayerInterface::ERROR == m_sourceID){
		std::cout << "run:reason=setSourceStreamFailed" << std::endl;
		return false;
	}
	
	std::cout << "newSourceId= " << m_sourceID << std::endl;

	this->play();

	return true;
}

bool AudioPlayerTest::run(std::string &url, std::chrono::milliseconds &offset){
	m_sourceID = m_playWrapper->setSource(url, offset);
	if(aisdk::utils::mediaPlayer::MediaPlayerInterface::ERROR == m_sourceID){
		std::cout << "run:reason=setSourceUrlFailed" << std::endl;
		return false;
	}
	std::cout << "newSourceId= " << m_sourceID << std::endl;
	if(flags) {
	//libai
	std::string url_test("http://yp.stormorai.cn/music/mp3/200556649.mp3");
	m_sourceStreamID = m_playStreamWrapper->setSource(url_test, offset);
	if(aisdk::utils::mediaPlayer::MediaPlayerInterface::ERROR == m_sourceID){
		std::cout << "run:reason=setSourceUrlFailed" << std::endl;
		return false;
	}
	
	std::cout << "newSourceId= " << m_sourceStreamID << std::endl;
	}
	
	this->play();
	
	return true;
}

bool AudioPlayerTest::play(){

	std::cout << "Playback with libao:" << std::endl << std::endl;

	std::cout << "Options: " << std::endl;
	std::cout << " p: play sound" << std::endl;
	std::cout << " r: resume sound" << std::endl;
	std::cout << " s: stop sound" << std::endl;
	std::cout << " z: pause sounds" << std::endl << std::endl;

	std::cout << "Press 'Q' to quit" << std::endl;

	int ch; 			
	bool done = false;
	while (!done)
	{
		if(!util::kbhit()) {
			ch = getchar();
			switch (ch) {
			case 'q':
			case 'Q':
					done = true;
					break;
			case 'p':
			case 'P':
					m_playWrapper->play(m_sourceID);
				if(flags) {
					m_playStreamWrapper->play(m_sourceStreamID);
				}
					break;
			case 'r':
			case 'R':
					m_playWrapper->resume(m_sourceID);
					break;
			case 's':
			case 'S':
					m_playWrapper->stop(m_sourceID);
				if(flags) {
					m_playStreamWrapper->stop(m_sourceStreamID);
				}
					break;
			case 'z':
			case 'Z':
					m_playWrapper->pause(m_sourceID);
					break;
			default:
					break;
			}
		}
	}
	
	m_playWrapper->shutdown();
	if(flags) {
		m_playStreamWrapper->shutdown();
	}
	if(!m_playWrapper){
		std::cout << "aoWrapperIsDestory" << std::endl;
	}
	
	return true;
}

void AudioPlayerTest::onPlaybackStarted(SourceId id){
	if(id == m_sourceID){
		std::cout << " Start..." << std::endl;
	}else
		std::cout << " source isnot match currentID: " << m_sourceID << "newId: " << id << std::endl;
}

void AudioPlayerTest::onPlaybackFinished(SourceId id){
	if(id == m_sourceID){
		std::cout << " Finished ..." << std::endl;
	}else
		std::cout << " source isnot match currentID: " << m_sourceID << "newId: " << id << std::endl;

}	

void AudioPlayerTest::onPlaybackError(SourceId id, const aisdk::utils::mediaPlayer::ErrorType& type, std::string error){
	if(id == m_sourceID){
		std::cout << " Error ..." << std::endl;
		auto errStr = aisdk::utils::mediaPlayer::errorTypeToString(type);
		std::cout << errStr << std::endl;
	}else
		std::cout << " source isnot match currentID: " << m_sourceID << "newId: " << id << std::endl;

}

void AudioPlayerTest::onPlaybackStopped(SourceId id){
	if(id == m_sourceID){
		std::cout << " stopped ..." << std::endl;
	}else
		std::cout << " source isnot match currentID: " << m_sourceID << "newId: " << id << std::endl;

}

void AudioPlayerTest::onPlaybackPaused(SourceId id){
	if(id == m_sourceID){
		std::cout << " paused ..." << std::endl;
	}else
		std::cout << " source isnot match currentID: " << m_sourceID << "newId: " << id << std::endl;

}

void AudioPlayerTest::onPlaybackResumed(SourceId id){
	if(id == m_sourceID){
		std::cout << " resumed ..." << std::endl;
	}else
		std::cout << " source isnot match currentID: " << m_sourceID << "newId: " << id << std::endl;

}

void PaHelp(){
	printf("Options: PaWrapperTest [options]\n" \
		"\t -u set play a url resource.\n" \
		"\t -f Set play a filename stream resource.\n" \
		"\t -p set url stream start position[uint: sec]\n" \
		"\t -r Set play a filename as attachment resource.\n" \
		"\t\t the resource format must be: s16_le,16000,mono\n");
}

int work(std::string &url, std::string &filename, std::string &attachment, std::chrono::milliseconds offset){
	auto audioPlayer = AudioPlayerTest::createNew();
	std::cout << "audioPlayer shared_ptr:user_count= " << audioPlayer.use_count() << std::endl;

	if(!url.empty()){
		std::cout << "offset: " << offset.count() << std::endl;
		audioPlayer->run(url, offset);
	}else if(!filename.empty()){
		std::shared_ptr<std::ifstream> input = std::make_shared<std::ifstream>();
		input->open(filename, std::ifstream::in);
		if(!input->is_open()){
			std::cout << "Open the file is failed\n" << std::endl;
			return -1;
		}
		audioPlayer->run(input);
	}else if (!attachment.empty()) {
		std::shared_ptr<std::ifstream> input = std::make_shared<std::ifstream>();
		input->open(attachment, std::ifstream::in);
		if(!input->is_open()){
			std::cout << "Open the file is failed\n" << std::endl;
			return -1;
		}
		auto attachmentReader = std::make_shared<MockAttachmentReader>(input);
		audioPlayer->run(attachmentReader);
	} else{
		audioPlayer.reset();
	}	
	
	std::cout << "audioPlayer shared_ptr:user_count= " << audioPlayer.use_count() << std::endl;
	audioPlayer.reset();
	
	std::cout << "play finished" << std::endl;

	return 0;
}

int main(int argc, char *argv[]){
	//std::shared_ptr<std::ifstream> input;
	std::string url;
	std::string filename;
	std::string attachmentName;	
	std::chrono::milliseconds offset = std::chrono::milliseconds::zero();
		
	int opt;
	
	while((opt = getopt(argc, argv, "yhu:r:p:f:")) != -1) {
	switch (opt) {
		case 'u':
			url = optarg;
			break;
		case 'p':
			offset = std::chrono::milliseconds(atoi(optarg)*1000);
			break;
		case 'f':
			filename = optarg;
			break;
		case 'r':
			attachmentName = optarg;
			break;			
		case 'h':
			PaHelp();
			exit(EXIT_FAILURE);
		case 'y':
			flags = 1;
			break;
		default:
			printf("optopt = %c\n", (char)optopt);
			printf("opterr = %d\n", opterr);
			fprintf(stderr, "usage: \n");
			PaHelp();
		   exit(EXIT_FAILURE);
	}
	}

	work(url, filename, attachmentName, offset);

	getchar();
	std::cout << "play finished exit==========" << std::endl;
}

