

#ifndef _CHANNEL_OBSERVER_INTERFACE_H_
#define _CHANNEL_OBSERVER_INTERFACE_H_

#include "Utils/Channel/FocusState.h"

namespace aisdk {
namespace utils {
namespace channel {

/**
 * A ChannelObserverInterface is an interface class that clients can extend to register for focus changes.
 */
class ChannelObserverInterface {
public:
    /**
     * Destructor.
     */
    virtual ~ChannelObserverInterface() = default;

    /**
     * Used to notify the observer of the Channel of track changes. 
     * @param newTrace The new Track of the channel.
     */
    virtual void onTrackChanged(FocusState newTrace) = 0;
};

}  // namespace channel
}  // namespace utils
}  // namespace aisdk

#endif  // _CHANNEL_OBSERVER_INTERFACE_H_
