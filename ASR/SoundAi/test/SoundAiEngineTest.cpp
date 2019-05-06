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

#include <iostream>
#include <fstream>
#include <string>

#include "SoundAi/SoundAiEngine.h"
#include "SoundAi/MessageConsumer.h"
#include "KeywordDetector/KeywordDetector.h"
#include "Utils/DialogRelay/DialogUXStateRelay.h"
#include "NLP/MessageInterpreter.h"
#include "NLP/DomainSequencer.h"
#include "AudioTrackManager/AudioTrackManager.h"

using namespace aisdk;

int main(int argc, char* argv[]){
	std::string config("kkk");
	std::string configPath("/cfg/sai_config");

	std::shared_ptr<utils::DeviceInfo> deviceInfo = utils::DeviceInfo::create(config);
	
	/// Creatint DomainSequencer object.
	auto domainSequencer = nlp::DomainSequencer::create();
	
	/// Creating the domain messageinterpreter.
	auto messageInterpreter = std::make_shared<nlp::MessageInterpreter>(std::move(domainSequencer));

	/// Creating a message consumer
	auto messageConsumer = std::make_shared<soundai::engine::MessageConsumer>(messageInterpreter);
		
	std::unique_ptr<soundai::engine::SoundAiEngine> instance = 
		soundai::engine::SoundAiEngine::create(deviceInfo, messageConsumer, configPath);
	
	if(!instance){
		std::cout << "create SoundAiEngine failed" << std::endl;
	}

	std::cout << "create SoundAiEngine success" << std::endl;

    /*
     * Creating the Audio Track Manager
     */
    auto audioTrackManager = std::make_shared<atm::AudioTrackManager>();
	

	auto keywordObserver = std::make_shared<kwd::KeywordDetector>(audioTrackManager);
	auto m_dialogUXStateRelay = std::make_shared<utils::dialogRelay::DialogUXStateRelay>();
	
	instance->addObserver(keywordObserver);
	instance->addObserver(m_dialogUXStateRelay);
	
	instance->start();
	
	getchar();
	instance->stop();
	instance.reset();
	
	return 0;
}


