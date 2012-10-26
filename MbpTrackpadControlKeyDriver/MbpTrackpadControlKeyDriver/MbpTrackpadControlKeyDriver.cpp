////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyDriver/MbpTrackpadControlKeyDriver.cpp

#include "./MbpTrackpadControlKeyDriver.h"
#include <IOKit/IOLib.h>
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
  
  OSIterator* sources = system_->getProviderIterator();
  if (sources) {
    int count = 0;
    while (OSObject* source = sources->getNextObject()) {
      // 最初に見つかった keyboard を MBP の keyboard と考える。
      if (OSDynamicCast(IOHIKeyboard, source)) {
        if (keyboard_ == NULL) {
          keyboard_ = reinterpret_cast<IOHIKeyboard*>(source);
        }
        ++count;
      }
    }
    sources->release();
    IOLog("MbpTrackpadControlKeyDriver found %d keyboard(s).\n", count);
  }

  registerService();
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void DRIVER_CLASS_NAME::stop(IOService* provider)
{
	IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
  
  system_ = NULL;
  keyboard_ = NULL;
  
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
