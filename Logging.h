////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKey/Logging.h

#if !defined(MBP_TRACKPAD_CONTROL_KEY__LOGGING_H)
#define      MBP_TRACKPAD_CONTROL_KEY__LOGGING_H

// compile unit の中で最後に include すること。
// 例えばもし、IOLog を宣言する <IOKit/IOLib.h> より前に include すると、NDEBUG 時
// IOLog の以下の宣言を置換した結果 compile error となる。
// 
// void IOLog(const char *format, ...)
// __attribute__((format(printf, 1, 2)));

#if !defined(NDEBUG)
#  if !defined(NSLog)
#    define NSLog( m, args... ) NSLog( m, ##args )
#  endif
#  if !defined(IOLog)
#    define IOLog( m, args... ) IOLog( m, ##args )
#  endif
#else
#  if !defined(NSLog)
#    define NSLog( m, args... )
#  endif
#  if !defined(IOLog)
#    define IOLog( m, args... )
#  endif
#endif

#endif