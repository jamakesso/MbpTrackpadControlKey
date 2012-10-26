////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyUtility/CTrackpadController.cpp

#include "CTrackpadController.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CTrackpadController

////////////////////////////////////////////////////////////////////////////////

CTrackpadController* CTrackpadController::instance__ = NULL;

////////////////////////////////////////////////////////////////////////////////

CTrackpadController::CTrackpadController(FHandler handler)
: handler_(handler)
, device_(NULL)
, isPressed_(false)
{
  assert(instance__ == NULL);
  assert(handler);
}

////////////////////////////////////////////////////////////////////////////////

CTrackpadController::~CTrackpadController()
{
  instance__ = NULL;
}

////////////////////////////////////////////////////////////////////////////////

bool CTrackpadController::initiate()
{
  instance__ = NULL;
  device_ = NULL;
  isPressed_ = false;

  MTDeviceRef device = MTDeviceCreateDefault();
  if (device == NULL) {
    return false;
  }

  instance__ = this;
  device_ = device;
  
  MTRegisterContactFrameCallback(device_, callback);
  MTDeviceStart(device_, 0);
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void CTrackpadController::terminate()
{
  MTDeviceStop(device_);
  MTDeviceRelease(device_);
  
  instance__ = NULL;
  device_ = NULL;
  isPressed_ = false;
}

////////////////////////////////////////////////////////////////////////////////

void CTrackpadController::setPressed(bool isPressed)
{
  isPressed_ = isPressed;
  
  handler_(isPressed);
}

////////////////////////////////////////////////////////////////////////////////

int CTrackpadController::callback(int device, Finger* data, int nFingers, double timestamp, int frame)
{
  assert(instance__ != NULL);
  
  CTrackpadController& tc = *instance__;
  
  if (nFingers == 0) {
    if (tc.isPressed()) {
      tc.setPressed(false);
    }
  }
  
  for (int i = 0; i < nFingers; ++i) {
    Finger* f = &data[i];
    if (0.95 < f->normalized.pos.y) {
      if (!tc.isPressed()) {
        tc.setPressed(true);
      }
    }
    else {
      if (tc.isPressed()) {
        tc.setPressed(false);
      }
    }
  }
  return 0;
}

