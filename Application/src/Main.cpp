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

#include "Application/SampleApp.h"
//#include "Application/AIClient.h"

//using namespace aisdk;
//using namespace application;

int main(int argc, char* argv[]) {
#if 1
	auto sampleApp = aisdk::application::SampleApp::createNew();
	if(!sampleApp) {
		std::cout << "Create FAILED!" << std::endl;
		return -1;
	}

	sampleApp->run();

	sampleApp.reset();
#else
	auto instance = aisdk::application::AIClient::createNew();
#endif

	return 0;
}

