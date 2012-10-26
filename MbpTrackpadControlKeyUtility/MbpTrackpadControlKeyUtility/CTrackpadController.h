////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyUtility/CTrackpadController.h

#if !defined(MBP_TRACKPAD_CONTROL_KEY_UTILITY__TRACKPAD_CONTROLLER_H) 
#define      MBP_TRACKPAD_CONTROL_KEY_UTILITY__TRACKPAD_CONTROLLER_H

#include "./Multipad.h"
#include <functional>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @class CTrackpadController
 * @brief
 */
class CTrackpadController
{
public:
  typedef std::function<void (bool isPressed)> FHandler;
  
  CTrackpadController(FHandler handler);
  ~CTrackpadController();

  bool initiate();
  void terminate();
  
  void setPressed(bool isPressed);
  bool isPressed() const { return isPressed_; }

private:
  static CTrackpadController* instance__;
  
  FHandler handler_;
  MTDeviceRef device_;
  bool isPressed_;
  
  static int callback(int device, Finger* data, int nFingers, double timestamp, int frame);
};

#endif
