////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyUtility/UserClientFacade.cpp

#include "./UserClientFacade.h"
#include "../../UserKernelShared.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CUserClientFacade

////////////////////////////////////////////////////////////////////////////////

kern_return_t CUserClientFacade::openUserClient(io_connect_t connect)
{
	return IOConnectCallScalarMethod(connect
                                   , USER_CLIENT_OPEN
                                   , NULL
                                   , 0
                                   , NULL
                                   , NULL);
}

////////////////////////////////////////////////////////////////////////////////

kern_return_t CUserClientFacade::closeUserClient(io_connect_t connect)
{
  return IOConnectCallScalarMethod(connect
                                   , USER_CLIENT_CLOSE
                                   , NULL
                                   , 0
                                   , NULL
                                   , NULL);
}

////////////////////////////////////////////////////////////////////////////////

kern_return_t CUserClientFacade::setControlModifierKey(io_connect_t connect, bool isActive)
{
  const uint64_t i = isActive ? 1 : 0;
  
  return IOConnectCallMethod(connect	                  // an io_connect_t returned from IOServiceOpen().
                             , SET_CONTROL_MODIFIER_KEY // selector of the function to be called via the user client.
                             , &i     // array of scalar (64-bit) input values.
                             , 1      // the number of scalar input values.
                             , NULL   // a pointer to the struct input parameter.
                             , 0      // the size of the input structure parameter.
                             , NULL   // array of scalar (64-bit) output values.
                             , NULL   // pointer to the number of scalar output values.
                             , NULL   // pointer to the struct output parameter.
                             , NULL); // pointer to the size of the output structure parameter.
}

