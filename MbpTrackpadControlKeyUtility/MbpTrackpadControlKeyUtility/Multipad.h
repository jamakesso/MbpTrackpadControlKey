////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyUtility/Multipad.h

#if !defined(MBP_TRACKPAD_CONTROL_KEY_UTILITY__MULTIPAD_H)
#define      MBP_TRACKPAD_CONTROL_KEY_UTILITY__MULTIPAD_H

// - MultitouchSupport.framework で定義される関数の prototype。
// - 同 framework は header file を提供しないので独自に定義する。
// - 捜せば apple が公式に release する API 仕様、及び header file があるのかも知れない
//   が、見つけることが出来なかった。
// - "VirtualAwesome" という open source の graphic library でこれらの定義を行っていた
//   ので、以下それから拝借して来た。
// - ただし、/// で comment out する部分については VirtualAwesome での定義は signature
//   が間違っていると思われ、適当に修正した。

extern "C" {
  
  typedef struct { float x,y; } mtPoint;
  typedef struct { mtPoint pos,vel; } mtReadout;
  
  typedef struct {
    int frame;
    double timestamp;
    int identifier, state, foo3, foo4;
    mtReadout normalized;
    float size;
    int zero1;
    float angle, majorAxis, minorAxis; // ellipsoid
    mtReadout mm;
    int zero2[2];
    float unk2;
  } Finger;
  
  ///typedef int MTDeviceRef;
  typedef void* MTDeviceRef;///
  typedef int (*MTContactCallbackFunction)(int,Finger*,int,double,int);
  
  MTDeviceRef MTDeviceCreateDefault();
  void MTRegisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
  void MTUnregisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
  ///void MTDeviceStart(MTDeviceRef);
  void MTDeviceStart(MTDeviceRef, int);///
  void MTDeviceStop(MTDeviceRef);
  void MTDeviceRelease(MTDeviceRef);
  
}

#endif
