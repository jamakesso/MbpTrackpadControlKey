////////////////////////////////////////////////////////////////////////////////
// MbpTrackpadControlKeyUtility/CMTCKAppDelegate.mm

#import "./CMTCKAppDelegate.h"
#include "./CTrackpadController.h"
#include "./UserClientFacade.h"
#include "../../UserKernelShared.h"
#include <IOKit/IOKitLib.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CMTCKAppDelegate

////////////////////////////////////////////////////////////////////////////////

@interface CMTCKAppDelegate ()
@property (weak) IBOutlet NSMenu *statusMenu;
@end

////////////////////////////////////////////////////////////////////////////////

@implementation CMTCKAppDelegate
{
  __weak NSMenu* _statusMenu;
  NSStatusItem* _statusItem;
  std::shared_ptr<CTrackpadController> _trackpadController;
  io_connect_t connect_;
}

////////////////////////////////////////////////////////////////////////////////

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  NSStatusBar* statusBar = [NSStatusBar systemStatusBar];
  
  _statusItem = [statusBar statusItemWithLength:NSVariableStatusItemLength];
  
  [_statusItem setTitle:@""];
  [_statusItem setImage:[NSImage imageNamed:@"icon1"]];
  [_statusItem setHighlightMode:YES];
  [_statusItem setMenu:_statusMenu];
  
  io_iterator_t iterator;
  if (IOServiceGetMatchingServices(kIOMasterPortDefault
                                   , IOServiceMatching(DRIVER_CLASS_NAME_STRING)
                                   , &iterator)
      != KERN_SUCCESS) {
    return;
  }
  
  connect_ = io_connect_t();
  io_service_t service;
  while ((service = IOIteratorNext(iterator)) != IO_OBJECT_NULL) {
    if (IOServiceOpen(service, mach_task_self(), 0, &connect_) != KERN_SUCCESS) {
      return;
    }
    if (CUserClientFacade().openUserClient(connect_) != KERN_SUCCESS) {
      return;
    }
	}
  IOObjectRelease(iterator);
  
	if (connect_ == io_connect_t()) {
    return;
	}
  
  typedef std::shared_ptr<CTrackpadController> TCSP;
  __weak CMTCKAppDelegate* weakSelf = self;
  _trackpadController
  = TCSP(new CTrackpadController
             ([weakSelf](bool isActive) {
                @autoreleasepool {
                  [weakSelf indicate:isActive];
                }
              }));
  
  _trackpadController->initiate();
}

////////////////////////////////////////////////////////////////////////////////

- (void)applicationWillTerminate:(NSNotification *)notification
{
  if (connect_ == IO_OBJECT_NULL) {
    return;
	}
	kern_return_t kr0 = CUserClientFacade().closeUserClient(connect_);
  kern_return_t kr1 = IOServiceClose(connect_);
  assert(kr0 == KERN_SUCCESS);
  assert(kr1 == KERN_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////

- (void)indicate:(BOOL)isPressed
{
  if (connect_ == IO_OBJECT_NULL) {
    return;
  }
  
  if (isPressed) {
    [_statusItem setImage:[NSImage imageNamed:@"icon2"]];
  }
  else {
    [_statusItem setImage:[NSImage imageNamed:@"icon1"]];
  }
  
  const kern_return_t kr = CUserClientFacade().setControlModifierKey(connect_, isPressed);
  assert(kr == KERN_SUCCESS);
}

@end
