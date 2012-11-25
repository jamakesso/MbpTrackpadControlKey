////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyDriver/MbpTrackpadControlKeyDriver.cpp

#include "./MbpTrackpadControlKeyDriver.h"
#include <IOKit/IOLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hidsystem/IOHIKeyboard.h>
#include <IOKit/hidsystem/IOHIDSystem.h>

#include "../../Logging.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyDriver

////////////////////////////////////////////////////////////////////////////////

OSDefineMetaClassAndStructors(jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyDriver, IOService)

////////////////////////////////////////////////////////////////////////////////

bool DRIVER_CLASS_NAME::init(OSDictionary *dictionary)
{
  if (!DSuper::init(dictionary)) {
    return false;
	}
  
	// This IOLog must follow super::init because getName relies on the superclass initialization.
	IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, dictionary);
	
  system_ = NULL;
  keyboard_ = NULL;
  
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void DRIVER_CLASS_NAME::free()
{
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
  
  DSuper::free();
}

////////////////////////////////////////////////////////////////////////////////

IOService* DRIVER_CLASS_NAME::probe(IOService* provider, SInt32* score)
{
	IOLog("%s[%p]::%s(%p, %p)\n", getName(), this, __FUNCTION__, provider, score);

  IOService *result = DSuper::probe(provider, score);
  
  return result;
}

////////////////////////////////////////////////////////////////////////////////

bool DRIVER_CLASS_NAME::start(IOService* provider)
{
	IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);

  system_ = NULL;
  keyboard_ = NULL;

  if (!DSuper::start(provider)) {
    return false;
  }
  
  system_ = IOHIDSystem::instance();
  if (system_ == NULL) {
    IOLog("error: IOHIDSystem not loaded.\n");
    return false;
  }

  if (!initiateNotification()) {
    IOLog("error: initiateNotification.\n");
    return false;
  }

  registerService();

  IOLog("DRIVER_CLASS_NAME::start succeeded.\n");
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void DRIVER_CLASS_NAME::stop(IOService* provider)
{
	IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
  
  system_ = NULL;
  keyboard_ = NULL;

  terminateNotification();

  DSuper::stop(provider);
}

////////////////////////////////////////////////////////////////////////////////

IOReturn DRIVER_CLASS_NAME::setControlModifierKey(bool isActive)
{
	IOLog("%s[%p]::%s(isActive = %d)\n"
        , getName()
        , this
        , __FUNCTION__
        , isActive);

  if (keyboard_ == NULL) {
    return kIOReturnSuccess;
  }
  
  enum {
    CONTROL_KEY = (NX_CONTROLMASK | NX_DEVICELCTLKEYMASK),
  };
  unsigned int dfs = keyboard_->deviceFlags();
  IOLog("device flags from = %d\n", dfs);
  if (isActive) {
    IOLog("control down\n");
    dfs |= CONTROL_KEY;
  }
  else {
    IOLog("control up\n");
    dfs &= ~CONTROL_KEY;
  }
  keyboard_->setDeviceFlags(dfs);
  IOLog("device flags to   = %d\n", keyboard_->deviceFlags());
  
  return kIOReturnSuccess;
}

////////////////////////////////////////////////////////////////////////////////

bool DRIVER_CLASS_NAME::keyboardMatchedNotificationHandler
(void* target
 , void* refCon
 , IOService* newService
 , IONotifier* notifier)
{
  IOLog("%s newService:%p\n", __FUNCTION__, newService);
  
  IOHIKeyboard* keyboard = OSDynamicCast(IOHIKeyboard, newService);
  if (!keyboard) {
    return false;
  }

  const OSNumber* vid = OSDynamicCast(OSNumber, keyboard->getProperty(kIOHIDVendorIDKey));
  const OSNumber* pid = OSDynamicCast(OSNumber, keyboard->getProperty(kIOHIDProductIDKey));
  const char* name = keyboard->getName();
  const OSString* manufacturer = OSDynamicCast(OSString, keyboard->getProperty(kIOHIDManufacturerKey));
  const OSString* product = OSDynamicCast(OSString, keyboard->getProperty(kIOHIDProductKey));

  IOLog("device: name         = %s\n", name);
  IOLog("device: manufacturer = %s\n", manufacturer->getCStringNoCopy());
  IOLog("device: product      = %s\n", product->getCStringNoCopy());
  IOLog("device: vender ID    = %lld\n", vid->unsigned64BitValue());
  IOLog("device: product ID   = %lld\n", pid->unsigned64BitValue());

  if (::strcmp(name, "IOHIDConsumer") == 0) {
    return false;
  }

  // 最初に見つかった keyboard を MBP の keyboard と考える。
  DRIVER_CLASS_NAME* driver = static_cast<DRIVER_CLASS_NAME*>(target);
  if (driver->keyboard_ == NULL) {
    driver->keyboard_ = keyboard;
    IOLog("MbpTrackpadControlKeyDriver has detected a keyboard.\n");
  }
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool DRIVER_CLASS_NAME::keyboardTerminatedNotificationHandler
(void* target
 , void* refCon
 , IOService* newService
 , IONotifier* notifier)
{
  IOLog("%s newService:%p\n", __FUNCTION__, newService);
  
  IOHIDevice* keyboard = OSDynamicCast(IOHIKeyboard, newService);
  if (!keyboard) {
    return false;
  }
  
  const char* name = keyboard->getName();
  if (::strcmp(name, "IOHIDConsumer") == 0) {
    return true;
  }
  
  DRIVER_CLASS_NAME* driver = static_cast<DRIVER_CLASS_NAME*>(target);
  if (driver->keyboard_ != NULL) {
    driver->keyboard_ = NULL;
    IOLog("MbpTrackpadControlKeyDriver has released the keyboard.\n");
  }
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool DRIVER_CLASS_NAME::initiateNotification()
{
  keyboardMatchedNotifier_
  = addMatchingNotification(gIOMatchedNotification
                            , serviceMatching("IOHIKeyboard")
                            , keyboardMatchedNotificationHandler
                            , this
                            , NULL
                            , 0);
  if (keyboardMatchedNotifier_ == NULL) {
    IOLog("initiateNotification keyboardMatchedNotifier_ == NULL\n");
    return false;
  }
  
  keyboardTerminatedNotifier_
  = addMatchingNotification(gIOTerminatedNotification
                            , serviceMatching("IOHIKeyboard")
                            , keyboardTerminatedNotificationHandler
                            , this
                            , NULL
                            , 0);
  if (keyboardTerminatedNotifier_ == NULL) {
    IOLog("initiateNotification keyboardTerminatedNotifier_ == NULL\n");
    return false;
  }
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void DRIVER_CLASS_NAME::terminateNotification()
{
  if (keyboardMatchedNotifier_ != NULL) {
    keyboardMatchedNotifier_->remove();
  }
  if (keyboardTerminatedNotifier_ != NULL) {
    keyboardTerminatedNotifier_->remove();
  }
}
