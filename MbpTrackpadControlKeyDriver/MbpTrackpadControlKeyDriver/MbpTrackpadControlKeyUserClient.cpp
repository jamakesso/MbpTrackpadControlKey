////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyUserClient/MbpTrackpadControlKeyUserClient.cpp

#include "./MbpTrackpadControlKeyUserClient.h"
#include <IOKit/IOKitKeys.h>
#include <IOKit/IOLib.h>
#include <libkern/OSByteOrder.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyUserClient

////////////////////////////////////////////////////////////////////////////////

OSDefineMetaClassAndStructors(jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyUserClient, IOUserClient)

////////////////////////////////////////////////////////////////////////////////

const IOExternalMethodDispatch USER_CLIENT_CLASS_NAME::methods__[METHOD_COUNT]
= {
  // USER_CLIENT_OPEN
	{
		(IOExternalMethodAction) &USER_CLIENT_CLASS_NAME::staticOpenUserClient	// method pointer.
		, 0	// no scalar input values.
		, 0	// no struct input value.
		, 0	// no scalar output values.
		, 0	// no struct output value.
	}
  // USER_CLIENT_CLOSE
	, {
		(IOExternalMethodAction) &USER_CLIENT_CLASS_NAME::staticCloseUserClient // method pointer.
		, 0	// no scalar input values.
		, 0	// no struct input value.
		, 0	// no scalar output values.
		, 0	// no struct output value.
	}
  // SET_CONTROL_MODIFIER_KEY
  , {
		(IOExternalMethodAction) &USER_CLIENT_CLASS_NAME::staticSetControlModifierKey // method pointer.
		, 1 // one scalar input value.
		, 0 // no struct input value.
		, 0 // no scalar output values.
		, 0 // no struct output value.
	}
};

////////////////////////////////////////////////////////////////////////////////

bool USER_CLIENT_CLASS_NAME::initWithTask
(task_t owningTask
 , void* securityToken
 , UInt32 type
 , OSDictionary* properties)
{
  bool	success;
  
	success = DSuper::initWithTask(owningTask, securityToken, type, properties);
	
	// This IOLog must follow DSuper::initWithTask because getName relies on the superclass initialization.
	IOLog("%s[%p]::%s(%p, %p, %d, %p)\n"
        , getName()
        , this
        , __FUNCTION__
        , owningTask
        , securityToken
        , type
        , properties);
  
	if (success) {
		// This code will do the right thing on both PowerPC- and Intel-based systems because the cross-endian
		// property will never be set on PowerPC-based Macs.
		isCrossEndian_ = false;
    
		if (properties != NULL && properties->getObject(kIOUserClientCrossEndianKey)) {
			// A connection to this user client is being opened by a user process running using Rosetta.
			
			// Indicate that this user client can handle being called from cross-endian user processes by
			// setting its IOUserClientCrossEndianCompatible property in the I/O Registry.
			if (setProperty(kIOUserClientCrossEndianCompatibleKey, kOSBooleanTrue)) {
				isCrossEndian_ = true;
				IOLog("%s[%p]::%s(): isCrossEndian_ = true\n", getName(), this, __FUNCTION__);
			}
		}
	}
	
  task_ = owningTask;
  provider_ = NULL;
  
  return success;
}

////////////////////////////////////////////////////////////////////////////////

bool USER_CLIENT_CLASS_NAME::start(IOService* provider)
{
  bool	success;
	
	IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
  
  // Verify that this user client is being started with a provider that it knows
	// how to communicate with.
	provider_ = OSDynamicCast(DRIVER_CLASS_NAME, provider);
  success = (provider_ != NULL);
  
  if (success) {
		// It's important not to call DSuper::start if some previous condition
		// (like an invalid provider) would cause this function to return false.
		// I/O Kit won't call stop on an object if its start function returned false.
		success = DSuper::start(provider);
	}
	
  return success;
}

////////////////////////////////////////////////////////////////////////////////

void USER_CLIENT_CLASS_NAME::stop(IOService* provider)
{
	IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
  
  DSuper::stop(provider);
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::clientClose(void)
{
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
  
  // Defensive coding in case the user process called IOServiceClose
	// without calling closeUserClient first.
  (void) closeUserClient();
  
	// Inform the user process that this user client is no longer available. This will also cause the
	// user client instance to be destroyed.
	//
	// terminate would return false if the user process still had this user client open.
	// This should never happen in our case because this code path is only reached if the user process
	// explicitly requests closing the connection to the user client.
	bool success = terminate();
	if (!success) {
		IOLog("%s[%p]::%s(): terminate() failed.\n", getName(), this, __FUNCTION__);
	}
  
  // DON'T call DSuper::clientClose, which just returns kIOReturnUnsupported.
  
  return kIOReturnSuccess;
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::clientDied(void)
{
  IOReturn result = kIOReturnSuccess;
  
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
  
  // The default implementation of clientDied just calls clientClose.
  result = DSuper::clientDied();
  
  return result;
}

////////////////////////////////////////////////////////////////////////////////

bool USER_CLIENT_CLASS_NAME::willTerminate(IOService* provider, IOOptionBits options)
{
	IOLog("%s[%p]::%s(%p, %d)\n", getName(), this, __FUNCTION__, provider, options);
	
	return DSuper::willTerminate(provider, options);
}

////////////////////////////////////////////////////////////////////////////////

bool USER_CLIENT_CLASS_NAME::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
	IOLog("%s[%p]::%s(%p, %d, %p)\n", getName(), this, __FUNCTION__, provider, options, defer);
	
	// If all pending I/O has been terminated, close our provider. If I/O is still outstanding, set defer to true
	// and the user client will not have stop called on it.
	closeUserClient();
	*defer = false;
	
	return DSuper::didTerminate(provider, options, defer);
}

////////////////////////////////////////////////////////////////////////////////

bool USER_CLIENT_CLASS_NAME::terminate(IOOptionBits options)
{
  bool	success;
  
	IOLog("%s[%p]::%s(%d)\n", getName(), this, __FUNCTION__, options);
  
  success = DSuper::terminate(options);
  
  return success;
}

////////////////////////////////////////////////////////////////////////////////

bool USER_CLIENT_CLASS_NAME::finalize(IOOptionBits options)
{
  bool	success;
  
	IOLog("%s[%p]::%s(%d)\n", getName(), this, __FUNCTION__, options);
  
  success = DSuper::finalize(options);
  
  return success;
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::externalMethod
(uint32_t selector
 , IOExternalMethodArguments* arguments
 , IOExternalMethodDispatch* dispatch
 , OSObject* target
 , void* reference)
{
	IOLog("%s[%p]::%s(%d, %p, %p, %p, %p)\n"
        , getName()
        , this
        , __FUNCTION__
        , selector
        , arguments
        , dispatch
        , target
        , reference);
  
  if (selector < (uint32_t) METHOD_COUNT) {
    dispatch = (IOExternalMethodDispatch *) &methods__[selector];
    if (!target) {
				target = this;
		}
  }
  
	return DSuper::externalMethod(selector, arguments, dispatch, target, reference);
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::staticOpenUserClient
(USER_CLIENT_CLASS_NAME* target
 , void* reference
 , IOExternalMethodArguments* arguments)
{
  return target->openUserClient();
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::staticCloseUserClient
(USER_CLIENT_CLASS_NAME* target
 , void* reference
 , IOExternalMethodArguments* arguments)
{
  return target->closeUserClient();
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::staticSetControlModifierKey
(USER_CLIENT_CLASS_NAME* target
 , void* reference
 , IOExternalMethodArguments* arguments)
{
  return target->setControlModifierKey(arguments->scalarInput[0]);
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::openUserClient()
{
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
  
  if (provider_ == NULL || isInactive()) {
    return kIOReturnNotAttached;
	}
  else if (!provider_->open(this)) {
		return kIOReturnExclusiveAccess;
	}
  
  return kIOReturnSuccess;
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::closeUserClient()
{
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
  
  if (provider_ == NULL) {
		IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
		return kIOReturnNotAttached;
	}
	else if (provider_->isOpen(this)) {
		provider_->close(this);
    return kIOReturnSuccess;
	}
	else {
		IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
		return kIOReturnNotOpen;
	}
}

////////////////////////////////////////////////////////////////////////////////

IOReturn USER_CLIENT_CLASS_NAME::setControlModifierKey(bool isActive)
{
	IOLog("%s[%p]::%s(isActive = %d)\n"
        , getName()
        , this
        , __FUNCTION__
        ,
        int(isActive));
	
	if (provider_ == NULL || isInactive()) {
		return kIOReturnNotAttached;
	}
	else if (!provider_->isOpen(this)) {
		return kIOReturnNotOpen;
	}
	else {
		return provider_->setControlModifierKey(isActive);
	}
}

