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
 
#ifndef __SAFE_SHUTDOWN_H_
#define __SAFE_SHUTDOWN_H_
#include <string>
#include <atomic>
#include <memory>
#include <mutex>

namespace aisdk {
namespace utils {

class SafeShutdown {
public:

	/**
     * Constructor.
     * @param name The name of the class or object which requires shutdown calls.
     */
	SafeShutdown(const std::string& name);

	/// Destructor.
	~SafeShutdown();
    /**
     * Returns the name of this object.
     *
     * @return The name of the object.
     */
    const std::string& name() const;

	/**
	 * Prepares/enables this object to be deleted.
	 */
	void shutdown();

	/**
	 * Checks whether this object has had @c shutdown() called on it.
	 *
	 * @return @c true if this object has been shut down, else @c false.
	 */
	bool isShutdown() const;

protected:
	/// @c shutdown() be called.
	virtual void doShutdown() = 0;

private:
	/// The name of the derived class.
	const std::string m_name;

	/// Mutex to protect shutdown.
	mutable std::mutex m_mutex;

	/// Flag tracking whether @c shutdown() has been called on this instance.
	bool m_isShutdown;

};
}	//utils
} // namespace aisdk

#endif //__SAFE_SHUTDOWN_H_
