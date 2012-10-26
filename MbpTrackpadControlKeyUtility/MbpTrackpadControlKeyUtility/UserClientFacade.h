////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyUtility/CUserClientFacade.h

#if !defined(MBP_TRACKPAD_CONTROL_KEY_UTILITY__USER_CLIENT_FACADE_H)
#define      MBP_TRACKPAD_CONTROL_KEY_UTILITY__USER_CLIENT_FACADE_H

#include <IOKit/IOKitLib.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @class CUserClientFacade
 * @brief
 */
class CUserClientFacade
{
public:
  kern_return_t openUserClient(io_connect_t connect);
  kern_return_t closeUserClient(io_connect_t connect);
  kern_return_t setControlModifierKey(io_connect_t connect, bool isActive);
};

#endif
