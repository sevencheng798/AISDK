/*
 * Copyright 2019 its affiliates. All Rights Reserved.
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
#include <string>
#include <vector>
#include <fstream>

#include "Utils/SharedBuffer/SharedBuffer.h"
#include "Application/PortAudioMicrophoneWrapper.h"

//#include "Application/AIClient.h"

//using namespace aisdk;
//using namespace application;

/// The sample rate of microphone audio data.
static const unsigned int SAMPLE_RATE_HZ = 16000;

/// The number of audio channels.
static const unsigned int NUM_CHANNELS = 1;

/// The size of each word within the stream.
static const size_t WORD_SIZE = 2;

/// The maximum number of readers of the stream.
static const size_t MAX_READERS = 10;

/// The amount of audio data to keep in the ring buffer.
static const std::chrono::seconds AMOUNT_OF_AUDIO_DATA_IN_BUFFER = std::chrono::seconds(15);

/// The size of the ring buffer.
static const size_t BUFFER_SIZE_IN_SAMPLES = (SAMPLE_RATE_HZ)*AMOUNT_OF_AUDIO_DATA_IN_BUFFER.count();

const std::chrono::milliseconds TIMEOUT_FOR_READ_CALLS = std::chrono::milliseconds(1000);

int main(int argc, char* argv[]) {
	std::cout << "create microphone test." << std::endl;
    size_t bufferSize = aisdk::utils::sharedbuffer::SharedBuffer::calculateBufferSize(
    BUFFER_SIZE_IN_SAMPLES, WORD_SIZE, MAX_READERS);

    auto buffer = std::make_shared<aisdk::utils::sharedbuffer::SharedBuffer::Buffer>(bufferSize);
    std::shared_ptr<aisdk::utils::sharedbuffer::SharedBuffer> sharedDataStream =
        aisdk::utils::sharedbuffer::SharedBuffer::create(buffer, WORD_SIZE, MAX_READERS);

    if (!sharedDataStream) {
        std::cout << "Failed to create shared data stream!" << std::endl;
        return false;
    }


	std::shared_ptr<aisdk::application::PortAudioMicrophoneWrapper> micWrapper 
		= aisdk::application::PortAudioMicrophoneWrapper::create(sharedDataStream);

	if(!micWrapper) {
		std::cout << "Failed to create microphone wrapper." << std::endl;
		return false;
	}

	micWrapper->startStreamingMicrophoneData();

	auto reader = sharedDataStream->createReader(aisdk::utils::sharedbuffer::Reader::Policy::BLOCKING);
	std::vector<uint16_t> audioDataToPush(320);
	std::ofstream output("/tmp/backup.pcm");
	std::cout << "stream wait....." << std::endl;
	getchar();
	micWrapper->stopStreamingMicrophoneData();
	while(1) {
		ssize_t wordsRead = reader->read(audioDataToPush.data(), audioDataToPush.size(), TIMEOUT_FOR_READ_CALLS);
		// Stream has been closed
		if(wordsRead == 0) {
			std::cout << "streamClosed." << std::endl;
		// This represents some sort of error with the read() call
		} else if(wordsRead < 0) {
			switch (wordsRead) {
            	case aisdk::utils::sharedbuffer::Reader::Error::OVERRUN:
					std::cout << "readerOverRun." << std::endl;
					reader->seek(0, aisdk::utils::sharedbuffer::Reader::Reference::BEFORE_WRITER);
				break;
				case aisdk::utils::sharedbuffer::Reader::Error::TIMEDOUT:
					std::cout << "readerTimeOut" << std::endl;
				break;
				default:
					std::cout << "unexpectedError" << std::endl;
					break;
			}
			// write data to file.
		} else if(wordsRead > 0) {
			auto buf8 = static_cast<void *>(audioDataToPush.data());
			output.write(static_cast<const char *>(buf8), wordsRead*(sizeof(*audioDataToPush.data())));
		}
		
	}
	
	return 0;
}

