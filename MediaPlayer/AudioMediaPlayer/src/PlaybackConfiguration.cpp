/*
 * Copyright 2018 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <list>
#include <unordered_map>
#include <iostream>

#include "AudioMediaPlayer/Endian.h"
#include "AudioMediaPlayer/PlaybackConfiguration.h"

namespace aisdk {
namespace mediaPlayer {
namespace ffmpeg {

/// Default playback sample rate is 48kHz.
static const size_t DEFAULT_SAMPLE_RATE{48000};

/// Default channel layout is stereo.
static const PlaybackConfiguration::ChannelLayout DEFAULT_LAYOUT{PlaybackConfiguration::ChannelLayout::LAYOUT_STEREO};

/// Default sample format is signed 16.
static const PlaybackConfiguration::SampleFormat DEFAULT_SAMPLE_FORMAT{PlaybackConfiguration::SampleFormat::SIGNED_16};

size_t PlaybackConfiguration::layoutToChannels(ChannelLayout layout) {
    switch (layout) {
        case ChannelLayout::LAYOUT_MONO:
            return 1;
        case ChannelLayout::LAYOUT_STEREO:
            return 2;
        case ChannelLayout::LAYOUT_SURROUND:
            return 3;
        case ChannelLayout::LAYOUT_5POINT1:
            return 6;
    }

	std::cout << "invalidLayout:" << "layout:" << static_cast<int>(layout) << std::endl;
    return 0;
}

PlaybackConfiguration::PlaybackConfiguration() :
        m_isLittleEndian{littleEndianMachine()},
        m_sampleRate{DEFAULT_SAMPLE_RATE},
        m_sampleFormat{DEFAULT_SAMPLE_FORMAT},
        m_layout{DEFAULT_LAYOUT},
        m_numberChannels{layoutToChannels(m_layout)} {
}

PlaybackConfiguration::PlaybackConfiguration(
    const bool isLittleEndian,
    const size_t sampleRate,
    const PlaybackConfiguration::ChannelLayout layout,
    const PlaybackConfiguration::SampleFormat sampleFormat) :
        m_isLittleEndian{isLittleEndian},
        m_sampleRate{sampleRate},
        m_sampleFormat{sampleFormat},
        m_layout{layout},
        m_numberChannels{layoutToChannels(m_layout)} {
}

}  // namespace ffmpeg
}  // namespace mediaPlayer
} //namespace aisdk