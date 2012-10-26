////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyUserClient/MbpTrackpadControlKeyUserClient.h

#if !defined(MBP_TRACKPAD_CONTROL_KEY_DRIVER__MBP_TRACKPAD_CONTROL_KEY_USER_CLIENT_H)
#define      MBP_TRACKPAD_CONTROL_KEY_DRIVER__MBP_TRACKPAD_CONTROL_KEY_USER_CLIENT_H

#include "./MbpTrackpadControlKeyDriver.h"
#include "../../UserKernelShared.h"
#include <IOKit/IOUserClient.h>

#define USER_CLIENT_CLASS_NAME jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyUserClient

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @class jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyUserClient
 * @brief
 */
class USER_CLIENT_CLASS_NAME : public IOUserClient
{
  OSDeclareDefaultStructors(jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyUserClient)
  
  typedef IOUserClient DSuper;
  
public:
  // IOUserClient interfaces.
  
	virtual bool initWithTask(task_t owningTask, void* securityToken, UInt32 type, OSDictionary* properties);
  
  virtual bool start(IOService* provider);
  virtual void stop(IOService* provider);
  
  virtual IOReturn clientClose(void);
  virtual IOReturn clientDied(void);
  
	virtual bool willTerminate(IOService* provider, IOOptionBits options);
	virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer);
	
  virtual bool terminate(IOOptionBits options = 0);
  virtual bool finalize(IOOptionBits options);
  
	// KPI for supporting access from both 32-bit and 64-bit user processes beginning with Mac OS X 10.5.
	virtual IOReturn externalMethod(uint32_t selector
                                  , IOExternalMethodArguments* arguments
                                  , IOExternalMethodDispatch* dispatch
                                  , OSObject* target
                                  , void* reference);

protected:
  static const IOExternalMethodDispatch	methods__[METHOD_COUNT];
	
  DRIVER_CLASS_NAME* provider_;
  task_t task_;     
	bool isCrossEndian_;

	static IOReturn staticOpenUserClient(USER_CLIENT_CLASS_NAME* target, void* reference, IOExternalMethodArguments* arguments);
	static IOReturn staticCloseUserClient(USER_CLIENT_CLASS_NAME* target, void* reference, IOExternalMethodArguments* arguments);
	static IOReturn staticSetControlModifierKey(USER_CLIENT_CLASS_NAME* target, void* reference, IOExternalMethodArguments* arguments);
  
	virtual IOReturn openUserClient();
  virtual IOReturn closeUserClient();
	virtual IOReturn setControlModifierKey(bool isActive);
};

#endif