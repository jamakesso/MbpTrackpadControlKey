////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyDriver/MbpTrackpadControlKeyDriver.h

#if !defined(MBP_TRACKPAD_CONTROL_KEY_DRIVER__MBP_TRACKPAD_CONTROL_KEY_DRIVER_H)
#define      MBP_TRACKPAD_CONTROL_KEY_DRIVER__MBP_TRACKPAD_CONTROL_KEY_DRIVER_H

#include "../../UserKernelShared.h"
#include <IOKit/IOService.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @class jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyDriver
 * @brief
 */
class IOHIDSystem;
class IOHIKeyboard;
class DRIVER_CLASS_NAME
: public IOService
{
  OSDeclareDefaultStructors(jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyDriver)
  
  typedef IOService DSuper;
  
public:
	// IOService interfaces.

  virtual bool init(OSDictionary* dictionary = 0);
  virtual void free();
  virtual IOService* probe(IOService* provider, SInt32* score);
  virtual bool start(IOService* provider);
  virtual void stop(IOService* provider);
  
  // local interfaces.
  
	virtual IOReturn setControlModifierKey(bool isActive);
  
private:
  IOHIDSystem* system_;
  IOHIKeyboard* keyboard_;
};

#endif