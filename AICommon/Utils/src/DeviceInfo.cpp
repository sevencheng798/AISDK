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
#include "Utils/DeviceInfo.h"

namespace aisdk {
namespace utils {

std::unique_ptr<DeviceInfo> DeviceInfo::create(std::string &configFile){
    std::string dialogId;
    std::string deviceSerialNumber;

	if(configFile.empty()){
		std::cout << "Device info config file is null" << std::endl;
		return nullptr;
	}

	/**
	 * To-Do Sven
	 * To implement how to generate the dialog of ID and the deviceSerialNumber
	 */
	// ...
	// ...
	// ...
	// The follow is a temporary definition. 
	dialogId = "123456789";
	deviceSerialNumber = "Sven-test";
	
	std::unique_ptr<DeviceInfo> instance(new DeviceInfo(dialogId, deviceSerialNumber));

	return instance;
}

std::string DeviceInfo::getDialogId() const {
    return m_dialogId;
}

std::string DeviceInfo::getDeviceSerialNumber() const {
    return m_deviceSerialNumber;
}

DeviceInfo::~DeviceInfo(){}

DeviceInfo::DeviceInfo(const std::string& dialogId, const std::string& deviceSerialNumber)
	:m_dialogId{dialogId}, m_deviceSerialNumber{deviceSerialNumber}{

}


}// utils
} // namespace aisdk