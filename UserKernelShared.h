////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKey/UserKernelShared.h

#if !defined(MBP_TRACKPAD_CONTROL_KEY__USER_KERNEL_SHARED_H)
#define      MBP_TRACKPAD_CONTROL_KEY__USER_KERNEL_SHARED_H

#define DRIVER_CLASS_NAME jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyDriver
#define DRIVER_CLASS_NAME_STRING "jp_or_iij4u_ss_yamaoka_MbpTrackpadControlKeyDriver"

// user client method dispatch selectors.
enum {
  USER_CLIENT_OPEN,
  USER_CLIENT_CLOSE,
  SET_CONTROL_MODIFIER_KEY,
  METHOD_COUNT,
};

#endif