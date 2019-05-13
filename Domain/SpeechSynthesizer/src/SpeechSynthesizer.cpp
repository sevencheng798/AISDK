/*
 * Copyright 2019 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <iostream>
// jsoncpp ver-1.8.3
#include <json/json.h>
#include <Utils/Logging/Logger.h>

#include "SpeechSynthesizer/SpeechSynthesizer.h"

/// String to identify log entries originating from this file.
static const std::string TAG("SpeechSynthesizer");
/// Define output
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace domain {
namespace speechSynthesizer {

using namespace utils::mediaPlayer;
using namespace utils::channel;
using namespace dmInterface;

/// The name of the @c AudioTrackManager channel used by the @c SpeechSynthesizer.
static const std::string CHANNEL_NAME = AudioTrackManagerInterface::DIALOG_CHANNEL_NAME;

/// The name of DomainProxy and SpeechChat handler interface
//static const std::string SPEECHCHAT{"SpeechChat"};

/// The name of the @c SafeShutdown
static const std::string SPEECHNAME{"SpeechSynthesizer"};

/// The duration to wait for a state change in @c onTrackChanged before failing.
static const std::chrono::seconds STATE_CHANGE_TIMEOUT{5};

/// The duration to start playing offset position.
static const std::chrono::milliseconds DEFAULT_OFFSET{0};

std::shared_ptr<SpeechSynthesizer> SpeechSynthesizer::create(
	std::shared_ptr<MediaPlayerInterface> mediaPlayer,
	std::shared_ptr<AudioTrackManagerInterface> trackManager,
	std::shared_ptr<utils::dialogRelay::DialogUXStateRelay> dialogUXStateRelay){
	if(!mediaPlayer){
		AISDK_ERROR(LX("SpeechCreationFailed").d("reason", "mediaPlayerNull"));
		return nullptr;
	}

	if(!trackManager){
		AISDK_ERROR(LX("SpeechCreationFailed").d("reason", "trackManagerNull"));
		return nullptr;
	}

	if(!dialogUXStateRelay){
		AISDK_ERROR(LX("SpeechCreationFailed").d("reason", "dialogUXStateRelayNull"));
		return nullptr;
	}

	auto instance = std::shared_ptr<SpeechSynthesizer>(new SpeechSynthesizer(mediaPlayer, trackManager));
	if(!instance){
		AISDK_ERROR(LX("SpeechCreationFailed").d("reason", "NewSpeechSynthesizerFailed"));
		return nullptr;
	}
	instance->init();

	dialogUXStateRelay->addObserver(instance);

	return instance;
}

void SpeechSynthesizer::onDeregistered() {
	// default no-op
}

void SpeechSynthesizer::preHandleDirective(std::shared_ptr<DirectiveInfo> info) {
	AISDK_INFO(LX("preHandleDirective").d("messageId", info->directive->getMessageId()));
    m_executor.submit([this, info]() { executePreHandle(info); });
}

void SpeechSynthesizer::handleDirective(std::shared_ptr<DirectiveInfo> info) {
	AISDK_INFO(LX("handleDirective").d("messageId", info->directive->getMessageId()));
    m_executor.submit([this, info]() { executeHandle(info); });
}

void SpeechSynthesizer::cancelDirective(std::shared_ptr<DirectiveInfo> info) {
	AISDK_INFO(LX("cancelDirective").d("messageId", info->directive->getMessageId()));
    m_executor.submit([this, info]() { executeCancel(info); });
}

void SpeechSynthesizer::onTrackChanged(FocusState newTrace) {
	AISDK_DEBUG5(LX("onTrackChanged").d("newTrace", newTrace));
    std::unique_lock<std::mutex> lock(m_mutex);
    m_currentFocus = newTrace;
    setDesiredStateLocked(newTrace);
    if (m_currentState == m_desiredState) {
        return;
    }
	
    // Set intermediate state to avoid being considered idle
    switch (newTrace) {
        case FocusState::FOREGROUND:
            setCurrentStateLocked(SpeechSynthesizerObserverInterface::SpeechSynthesizerState::GAINING_FOCUS);
            break;
        case FocusState::BACKGROUND:
            setCurrentStateLocked(SpeechSynthesizerObserverInterface::SpeechSynthesizerState::LOSING_FOCUS);
            break;
        case FocusState::NONE:
            // We do not care of other track focus states yet
            break;
    }

	auto messageId = (m_currentInfo && m_currentInfo->directive) ? m_currentInfo->directive->getMessageId() : "";
    m_executor.submit([this]() { executeStateChange(); });

	// Block until we achieve the desired state.
    if (m_waitOnStateChange.wait_for(
            lock, STATE_CHANGE_TIMEOUT, [this]() { return m_currentState == m_desiredState; })) {
		AISDK_INFO(LX("onTrackChangedSuccess"));
    } else {
		AISDK_ERROR(LX("onFocusChangeFailed").d("reason", "stateChangeTimeout").d("messageId", messageId));
        if (m_currentInfo) {
            lock.unlock();
			reportExceptionFailed(m_currentInfo, "stateChangeTimeout");
        }
    }
}

void SpeechSynthesizer::onPlaybackStarted(SourceId id) {
	AISDK_INFO(LX("onPlaybackStarted").d("callbackSourceId", id));
    if (id != m_mediaSourceId) {
		AISDK_ERROR(LX("queueingExecutePlaybackStartedFailed")
					.d("reason", "mismatchSourceId")
					.d("callbackSourceId", id)
					.d("sourceId", m_mediaSourceId));
        m_executor.submit([this] {
            executePlaybackError(ErrorType::MEDIA_ERROR_INTERNAL_DEVICE_ERROR, "executePlaybackStartedFailed");
        });
    } else {
        m_executor.submit([this]() { executePlaybackStarted(); });
    }

}

void SpeechSynthesizer::onPlaybackFinished(SourceId id) {
	AISDK_INFO(LX("onPlaybackFinished").d("callbackSourceId", id));

    if (id != m_mediaSourceId) {
		AISDK_ERROR(LX("queueingExecutePlaybackFinishedFailed")
					.d("reason", "mismatchSourceId")
					.d("callbackSourceId", id)
					.d("sourceId", m_mediaSourceId));
		m_executor.submit([this] {
            executePlaybackError(ErrorType::MEDIA_ERROR_INTERNAL_DEVICE_ERROR, "executePlaybackFinishedFailed");
        });
    } else {
        m_executor.submit([this]() { executePlaybackFinished(); });
    }
}

void SpeechSynthesizer::onPlaybackError(
	SourceId id,
	const utils::mediaPlayer::ErrorType& type,
	std::string error) {
	AISDK_INFO(LX("onPlaybackError").d("callbackSourceId", id));
    m_executor.submit([this, type, error]() { executePlaybackError(type, error); });
}

void SpeechSynthesizer::onPlaybackStopped(SourceId id) {
	AISDK_INFO(LX("onPlaybackStopped").d("callbackSourceId", id));
    onPlaybackFinished(id);
}

void SpeechSynthesizer::addObserver(std::shared_ptr<SpeechSynthesizerObserverInterface> observer) {
	AISDK_INFO(LX("addObserver").d("observer", observer.get()));
    m_executor.submit([this, observer]() { m_observers.insert(observer); });
}

void SpeechSynthesizer::removeObserver(std::shared_ptr<SpeechSynthesizerObserverInterface> observer) {
	AISDK_INFO(LX("removeObserver").d("observer", observer.get()));
    m_executor.submit([this, observer]() { m_observers.erase(observer); }).wait();
}

std::string SpeechSynthesizer::getHandlerName() const {
	return m_handlerName;
}

void SpeechSynthesizer::doShutdown() {
	AISDK_INFO(LX("doShutdown").d("reason", "destory"));
	m_speechPlayer->setObserver(nullptr);
	{
        std::unique_lock<std::mutex> lock(m_mutex);
        if (SpeechSynthesizerObserverInterface::SpeechSynthesizerState::PLAYING == m_currentState ||
            SpeechSynthesizerObserverInterface::SpeechSynthesizerState::PLAYING == m_desiredState) {
            m_desiredState = SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED;
            stopPlaying();
            m_currentState = SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED;
            lock.unlock();
            releaseForegroundTrace();
        }
    }
	
    {
        std::lock_guard<std::mutex> lock(m_chatInfoQueueMutex);
        for (auto& info : m_chatInfoQueue) {
            if (info.get()->result) {
                info.get()->result->setFailed("SpeechSynthesizerShuttingDown");
            }
            removeChatDirectiveInfo(info.get()->directive->getMessageId());
            removeDirective(info.get()->directive->getMessageId());
        }
    }
	
    m_executor.shutdown();
    m_speechPlayer.reset();
    m_waitOnStateChange.notify_one();
    m_trackManager.reset();
    m_observers.clear();

}

SpeechSynthesizer::ChatDirectiveInfo::ChatDirectiveInfo(
	std::shared_ptr<nlp::DomainProxy::DirectiveInfo> directiveInfo) :
	directive{directiveInfo->directive},
	result{directiveInfo->result},
	sendCompletedMessage{false} {
}
	
void SpeechSynthesizer::ChatDirectiveInfo::clear() {
	attachmentReader.reset();
    sendCompletedMessage = false;
}

SpeechSynthesizer::SpeechSynthesizer(
	std::shared_ptr<MediaPlayerInterface> mediaPlayer,
	std::shared_ptr<AudioTrackManagerInterface> trackManager) :
	DomainProxy{SPEECHNAME},
	SafeShutdown{SPEECHNAME},
	m_handlerName{SPEECHNAME},
	m_speechPlayer{mediaPlayer},
	m_trackManager{trackManager},
	m_mediaSourceId{MediaPlayerInterface::ERROR},
	m_currentState{SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED},
	m_desiredState{SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED},
	m_currentFocus{FocusState::NONE},
	m_isAlreadyStopping{false} {
}

void SpeechSynthesizer::init() {
    m_speechPlayer->setObserver(shared_from_this());
}

void SpeechSynthesizer::executePreHandleAfterValidation(std::shared_ptr<ChatDirectiveInfo> info) {

	// TODO:parse tts url and insert chatInfo map - Fix me.
#ifdef ENABLE_SOUNDAI_ASR
	auto data = info->directive->getData();
	Json::CharReaderBuilder readerBuilder;
	JSONCPP_STRING errs;
	Json::Value root;
	std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());
	if (!reader->parse(data.c_str(), data.c_str()+data.length(), &root, &errs)) {
		AISDK_ERROR(LX("executePreHandleAfterValidation").d("reason", "parseDataKeyError"));
		return;
	}
	std::string url = root["tts_url"].asString();
	AISDK_INFO(LX("executePreHandleAfterValidation").d("url", url));
	info->url = url;
#else	
	info->attachmentReader = info->directive->getAttachmentReader(
		info->directive->getMessageId(), utils::sharedbuffer::ReaderPolicy::BLOCKING);
#endif	
	// If everything checks out, add the chatInfo to the map.
    if (!setChatDirectiveInfo(info->directive->getMessageId(), info)) {
		AISDK_ERROR(LX("executePreHandleFailed")
					.d("reason", "prehandleCalledTwiceOnSameDirective")
					.d("messageId", info->directive->getMessageId()));
    }
}

void SpeechSynthesizer::executeHandleAfterValidation(std::shared_ptr<ChatDirectiveInfo> info) {
    m_currentInfo = info;
    if (!m_trackManager->acquireChannel(CHANNEL_NAME, shared_from_this(), SPEECHNAME)) {
        static const std::string message = std::string("Could not acquire ") + CHANNEL_NAME + " for " + SPEECHNAME;
		AISDK_INFO(LX("executeHandleFailed")
					.d("reason", "CouldNotAcquireChannel")
					.d("messageId", m_currentInfo->directive->getMessageId()));
        reportExceptionFailed(info, message);
    }
}

void SpeechSynthesizer::executePreHandle(std::shared_ptr<DirectiveInfo> info) {
	AISDK_INFO(LX("executePreHandle").d("messageId", info->directive->getMessageId()));
    auto chatInfo = validateInfo("executePreHandle", info);
    if (!chatInfo) {
		AISDK_ERROR(LX("executePreHandleFailed").d("reason", "invalidDirectiveInfo"));
        return;
    }
    executePreHandleAfterValidation(chatInfo);
}

void SpeechSynthesizer::executeHandle(std::shared_ptr<DirectiveInfo> info) {
	AISDK_INFO(LX("executeHandle").d("messageId", info->directive->getMessageId()));
    auto chatInfo = validateInfo("executeHandle", info);
    if (!chatInfo) {
		AISDK_ERROR(LX("executeHandleFailed").d("reason", "invalidDirectiveInfo"));
        return;
    }
    addToDirectiveQueue(chatInfo);
}

void SpeechSynthesizer::executeCancel(std::shared_ptr<DirectiveInfo> info) {
	AISDK_INFO(LX("executeCancel").d("messageId", info->directive->getMessageId()));
    auto chatInfo = validateInfo("executeCancel", info);
    if (!chatInfo) {
		AISDK_ERROR(LX("executeCancelFailed").d("reason", "invalidDirectiveInfo"));
        return;
    }
    if (chatInfo != m_currentInfo) {
        chatInfo->clear();
        removeChatDirectiveInfo(chatInfo->directive->getMessageId());
        {
            std::lock_guard<std::mutex> lock(m_chatInfoQueueMutex);
            for (auto it = m_chatInfoQueue.begin(); it != m_chatInfoQueue.end(); it++) {
                if (chatInfo->directive->getMessageId() == it->get()->directive->getMessageId()) {
                    it = m_chatInfoQueue.erase(it);
                    break;
                }
            }
        }
        removeDirective(chatInfo->directive->getMessageId());
        return;
    }
	
    std::unique_lock<std::mutex> lock(m_mutex);
    if (SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED != m_desiredState) {
        m_desiredState = SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED;
        if (SpeechSynthesizerObserverInterface::SpeechSynthesizerState::PLAYING == m_currentState ||
            SpeechSynthesizerObserverInterface::SpeechSynthesizerState::GAINING_FOCUS == m_currentState) {
            lock.unlock();
            if (m_currentInfo) {
                m_currentInfo->sendCompletedMessage = false;
            }
            stopPlaying();
        }
    }

}

void SpeechSynthesizer::executeStateChange() {
    SpeechSynthesizerObserverInterface::SpeechSynthesizerState newState;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        newState = m_desiredState;
    }

	AISDK_INFO(LX("executeStateChange").d("newState", newState));
    switch (newState) {
        case SpeechSynthesizerObserverInterface::SpeechSynthesizerState::PLAYING:
            if (m_currentInfo) {
                m_currentInfo->sendCompletedMessage = true;
            }
			// Trigger play
            startPlaying();
            break;
        case SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED:
			// Trigger stop
            stopPlaying();
            break;
        case SpeechSynthesizerObserverInterface::SpeechSynthesizerState::GAINING_FOCUS:
        case SpeechSynthesizerObserverInterface::SpeechSynthesizerState::LOSING_FOCUS:
            // Nothing to do
            break;
    }
}

void SpeechSynthesizer::executePlaybackStarted() {
	AISDK_INFO(LX("executePlaybackStarted"));
	
    if (!m_currentInfo) {
		AISDK_ERROR(LX("executePlaybackStartedIgnored").d("reason", "nullptrDirectiveInfo"));
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_mutex);
		/// Set current state @c m_currentState to PLAYING to specify device alreay start to playback.
        setCurrentStateLocked(SpeechSynthesizerObserverInterface::SpeechSynthesizerState::PLAYING);
    }
	
	/// wakeup condition wait
    m_waitOnStateChange.notify_one();
}

void SpeechSynthesizer::executePlaybackFinished() {
	AISDK_INFO(LX("executePlaybackFinished"));

    if (!m_currentInfo) {
		AISDK_ERROR(LX("executePlaybackFinishedIgnored").d("reason", "nullptrDirectiveInfo"));
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        setCurrentStateLocked(SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED);
    }
    m_waitOnStateChange.notify_one();

    if (m_currentInfo->sendCompletedMessage) {
        setHandlingCompleted();
    }
	
    resetCurrentInfo();
	
    {
        std::lock_guard<std::mutex> lock_guard(m_chatInfoQueueMutex);
        m_chatInfoQueue.pop_front();
        if (!m_chatInfoQueue.empty()) {
            executeHandleAfterValidation(m_chatInfoQueue.front());
        }
    }
	
    resetMediaSourceId();

}

void SpeechSynthesizer::executePlaybackError(const utils::mediaPlayer::ErrorType& type, std::string error) {
	AISDK_INFO(LX("executePlaybackError").d("type", type).d("error", error));
    if (!m_currentInfo) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        setCurrentStateLocked(SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED);
    }
    m_waitOnStateChange.notify_one();
    releaseForegroundTrace();
    resetCurrentInfo();
    resetMediaSourceId();
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_chatInfoQueue.empty()) {
			auto charInfo = m_chatInfoQueue.front();
            m_chatInfoQueue.pop_front();
			lock.unlock();
			reportExceptionFailed(charInfo, error);
			lock.lock();
        }
    }

}

void SpeechSynthesizer::startPlaying() {
	AISDK_INFO(LX("startPlaying"));
	#ifndef ENABLE_SOUNDAI_ASR
	/// The following params must be set fix point.
	const utils::AudioFormat format{
						.encoding = aisdk::utils::AudioFormat::Encoding::LPCM,
					   .endianness = aisdk::utils::AudioFormat::Endianness::LITTLE,
					   .sampleRateHz = 16000,	
					   .sampleSizeInBits = 16,
					   .numChannels = 1,	
					   .dataSigned = true
	};
	
    m_mediaSourceId = m_speechPlayer->setSource(std::move(m_currentInfo->attachmentReader), &format);
	#else
	m_mediaSourceId = m_speechPlayer->setSource(m_currentInfo->url);
	#endif
    if (MediaPlayerInterface::ERROR == m_mediaSourceId) {
		AISDK_ERROR(LX("startPlayingFailed").d("reason", "setSourceFailed"));
        executePlaybackError(ErrorType::MEDIA_ERROR_INTERNAL_DEVICE_ERROR, "playFailed");
    } else if (!m_speechPlayer->play(m_mediaSourceId)) {
        executePlaybackError(ErrorType::MEDIA_ERROR_INTERNAL_DEVICE_ERROR, "playFailed");
    } else {
        // Execution of play is successful.
        m_isAlreadyStopping = false;
    }
}

void SpeechSynthesizer::stopPlaying() {
	AISDK_INFO(LX("stopPlaying"));
    if (MediaPlayerInterface::ERROR == m_mediaSourceId) {
		AISDK_ERROR(LX("stopPlayingFailed").d("reason", "invalidMediaSourceId").d("mediaSourceId", m_mediaSourceId));
    } else if (m_isAlreadyStopping) {
		AISDK_WARN(LX("stopPlayingIgnored").d("reason", "isAlreadyStopping"));
    } else if (!m_speechPlayer->stop(m_mediaSourceId)) {
        executePlaybackError(ErrorType::MEDIA_ERROR_INTERNAL_DEVICE_ERROR, "stopFailed");
    } else {
        // Execution of stop is successful.
        m_isAlreadyStopping = true;
    }
}

void SpeechSynthesizer::setCurrentStateLocked(
	SpeechSynthesizerObserverInterface::SpeechSynthesizerState newState) {
	AISDK_INFO(LX("setCurrentStateLocked").d("state", newState));

    m_currentState = newState;

    for (auto observer : m_observers) {
        observer->onStateChanged(m_currentState);
    }
}

void SpeechSynthesizer::setDesiredStateLocked(FocusState newTrace) {
    switch (newTrace) {
        case FocusState::FOREGROUND:
            m_desiredState = SpeechSynthesizerObserverInterface::SpeechSynthesizerState::PLAYING;
            break;
        case FocusState::BACKGROUND:
        case FocusState::NONE:
            m_desiredState = SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED;
            break;
    }
}

void SpeechSynthesizer::resetCurrentInfo(std::shared_ptr<ChatDirectiveInfo> chatInfo) {
    if (m_currentInfo != chatInfo) {
        if (m_currentInfo) {
            removeChatDirectiveInfo(m_currentInfo->directive->getMessageId());
			// Removing map of @c DomainProxy's @c m_directiveInfoMap
            removeDirective(m_currentInfo->directive->getMessageId());
            m_currentInfo->clear();
        }
        m_currentInfo = chatInfo;
    }
}

void SpeechSynthesizer::setHandlingCompleted() {
	AISDK_INFO(LX("setHandlingCompleted"));
    if (m_currentInfo && m_currentInfo->result) {
        m_currentInfo->result->setCompleted();
    }
}

void SpeechSynthesizer::reportExceptionFailed(
	std::shared_ptr<ChatDirectiveInfo> info,
	const std::string& message) {
    if (info && info->result) {
        info->result->setFailed(message);
    }
    info->clear();
    removeDirective(info->directive->getMessageId());
    std::unique_lock<std::mutex> lock(m_mutex);
    if (SpeechSynthesizerObserverInterface::SpeechSynthesizerState::PLAYING == m_currentState ||
        SpeechSynthesizerObserverInterface::SpeechSynthesizerState::GAINING_FOCUS == m_currentState) {
        lock.unlock();
        stopPlaying();
    }
}

void SpeechSynthesizer::releaseForegroundTrace() {
	AISDK_INFO(LX("releaseForegroundTrace"));
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentFocus = FocusState::NONE;
    }
    m_trackManager->releaseChannel(CHANNEL_NAME, shared_from_this());
}

void SpeechSynthesizer::resetMediaSourceId() {
    m_mediaSourceId = MediaPlayerInterface::ERROR;
}

std::shared_ptr<SpeechSynthesizer::ChatDirectiveInfo> SpeechSynthesizer::validateInfo(
	const std::string& caller,
	std::shared_ptr<DirectiveInfo> info,
	bool checkResult) {
    if (!info) {
		AISDK_ERROR(LX(caller + "Failed").d("reason", "nullptrInfo"));
        return nullptr;
    }
    if (!info->directive) {
		AISDK_ERROR(LX(caller + "Failed").d("reason", "nullptrDirective"));
        return nullptr;
    }
    if (checkResult && !info->result) {
		AISDK_ERROR(LX(caller + "Failed").d("reason", "nullptrResult"));
        return nullptr;
    }

    auto chatDirInfo = getChatDirectiveInfo(info->directive->getMessageId());
    if (chatDirInfo) {
        return chatDirInfo;
    }

    chatDirInfo = std::make_shared<ChatDirectiveInfo>(info);

    return chatDirInfo;

}

std::shared_ptr<SpeechSynthesizer::ChatDirectiveInfo> SpeechSynthesizer::getChatDirectiveInfo(const std::string& messageId) {
    std::lock_guard<std::mutex> lock(m_chatDirectiveInfoMutex);
    auto it = m_chatDirectiveInfoMap.find(messageId);
    if (it != m_chatDirectiveInfoMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool SpeechSynthesizer::setChatDirectiveInfo(
	const std::string& messageId,
	std::shared_ptr<SpeechSynthesizer::ChatDirectiveInfo> info) {
	std::lock_guard<std::mutex> lock(m_chatDirectiveInfoMutex);
    auto it = m_chatDirectiveInfoMap.find(messageId);
    if (it != m_chatDirectiveInfoMap.end()) {
        return false;
    }
    m_chatDirectiveInfoMap[messageId] = info;
    return true;
}

void SpeechSynthesizer::addToDirectiveQueue(std::shared_ptr<ChatDirectiveInfo> info) {
    std::lock_guard<std::mutex> lock(m_chatInfoQueueMutex);
    if (m_chatInfoQueue.empty()) {
        m_chatInfoQueue.push_back(info);
        executeHandleAfterValidation(info);
    } else {
		AISDK_INFO(LX("addToDirectiveQueue").d("queueSize", m_chatInfoQueue.size()));
        m_chatInfoQueue.push_back(info);
    }
}

void SpeechSynthesizer::removeChatDirectiveInfo(const std::string& messageId) {
    std::lock_guard<std::mutex> lock(m_chatDirectiveInfoMutex);
    m_chatDirectiveInfoMap.erase(messageId);
}

void SpeechSynthesizer::onDialogUXStateChanged(
	utils::dialogRelay::DialogUXStateObserverInterface::DialogUXState newState) {
	AISDK_DEBUG5(LX("onDialogUXStateChanged"));
	m_executor.submit([this, newState]() { executeOnDialogUXStateChanged(newState); });
}

void SpeechSynthesizer::executeOnDialogUXStateChanged(
    utils::dialogRelay::DialogUXStateObserverInterface::DialogUXState newState) {
	AISDK_INFO(LX("executeOnDialogUXStateChanged"));
    if (newState != utils::dialogRelay::DialogUXStateObserverInterface::DialogUXState::IDLE) {
        return;
    }
    if (m_currentFocus != FocusState::NONE &&
        m_currentState != SpeechSynthesizerObserverInterface::SpeechSynthesizerState::GAINING_FOCUS) {
        m_trackManager->releaseChannel(CHANNEL_NAME, shared_from_this());
        m_currentFocus = FocusState::NONE;
    }
}

}	// namespace speechSynthesizer
}	// namespace domain
}	// namespace aisdk
