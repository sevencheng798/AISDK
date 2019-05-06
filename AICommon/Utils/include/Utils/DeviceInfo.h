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
 
#ifndef __DEVICE_INFO_H_
#define __DEVICE_INFO_H_
#include <memory>

namespace aisdk {
namespace utils {

class DeviceInfo {
public:
	/**
     * Create a DeviceInfo.
     *
     * @param configFile The restore info config file.
     * @return If successful, returns a new DeviceInfo, otherwise @c nullptr.
     */
	static std::unique_ptr<DeviceInfo> create(std::string &configFile);

	/**
     * Gets the Dialog Id.
     *
     * @return Dialog Id.
     */
    std::string getDialogId() const;

    /**
     * Gets the DSN.
     *
     * @return DSN.
     */
    std::string getDeviceSerialNumber() const;
	
    /**
     * Destructor.
     */
	~DeviceInfo();

private:
	/**
     * Constructor.
     */
	DeviceInfo(const std::string& dialogId, const std::string& deviceSerialNumber);

    /// Dialog ID
    std::string m_dialogId;

    /// DSN
    std::string m_deviceSerialNumber;
};
}	//utils
} // namespace aisdk
#endif //__DEVICE_INFO_H_

