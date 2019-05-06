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
#include <mutex>
#include <thread>

#include <portaudio.h>
#include <Utils/SharedBuffer/SharedBuffer.h>
#include <Utils/Microphone/MicrophoneInterface.h>

namespace aisdk {
namespace application {

/// This acts as a wrapper around PortAudio library api.
class PortAudioMicrophoneWrapper : public utils::microphone::MicrophoneInterface {
public:
	/**
	 * Create a @c PortAudioMicrophoneWrapper.
	 * @params stream The shared data stream to write to.
	 * @return @c nullptr if create failed and a unique_ptr to a @c PortAudioMicrophoneWrapper otherwise.
	 */
	static std::unique_ptr<PortAudioMicrophoneWrapper> create(
			std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream);

	/// @name MicrophoneInterface method:
	/// @{
	/**
     * Stops streaming from the microphone.
     *
     * @return Whether the stop was successful.
     */
    bool stopStreamingMicrophoneData() override;

    /**
     * Starts streaming from the microphone.
     *
     * @return Whether the start was successful.
     */
    bool startStreamingMicrophoneData() override;
	/// @}
	
    /**
     * Destructor.
     */
    ~PortAudioMicrophoneWrapper();
private:
	/**
	 * Constructor.
	 * @params stream The shared data stream to write to.
	 */
	PortAudioMicrophoneWrapper(
		std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream);

	/// The callback that PortAudio will issue when audio is avaiable to read.
	static int PortAudioCallback(
		const void *inputBuffer,
		void *outputBuffer,
		unsigned long numSamples,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);

	/// Initialize portaudio library.
	bool initialize();

	/// The stream of data.
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> m_audioInputStream;

	/// The writer will be used to write audio data to share buffer.
	std::shared_ptr<utils::sharedbuffer::Writer> m_writer;

	/// The PortAudio stream
    PaStream* m_paStream;

    /**
     * A lock to seralize access to startStreamingMicrophoneData() and stopStreamingMicrophoneData() between different
     * threads.
     */
    std::mutex m_mutex;
};

} // namespace application
} // namespace aisdk
