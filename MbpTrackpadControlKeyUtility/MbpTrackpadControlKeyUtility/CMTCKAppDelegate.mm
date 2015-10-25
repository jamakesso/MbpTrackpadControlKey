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
  // setup status bar.
  
  NSStatusBar* statusBar = [NSStatusBar systemStatusBar];
  
  _statusItem = [statusBar statusItemWithLength:NSVariableStatusItemLength];
  
  [_statusItem setTitle:@""];
  [_statusItem setImage:[NSImage imageNamed:@"icon1"]];
  [_statusItem setHighlightMode:YES];
  [_statusItem setMenu:_statusMenu];
  
  // create trackpad controller.
  
  typedef std::shared_ptr<CTrackpadController> TCSP;
  __weak CMTCKAppDelegate* weakSelf = self;
  _trackpadController
  = TCSP(new CTrackpadController(
      [weakSelf](bool isActive) {
        @autoreleasepool {
          [weakSelf handleTrackpadEvent:isActive];
        }
      }
    ));

  // register sleep, wake notification.
  
  [self registerSleepWakeNotifications];
  
  // parallel desktop 上での動作では touchpad を取得出来ないので以下で driver を test する。
#if 0
  [self initiateTrackpadEventHandling];
  
  BOOL pressing = NO;
  while (true) {
    pressing = !pressing;
    if (pressing) {
      NSLog(@"press on");
    }
    else {
      NSLog(@"press off");
    }
    [self handleTrackpadEvent:pressing];
    sleep(3);
  }
#endif

  // start event handling.
  
  if (![self initiateTrackpadEventHandling]) {
    [NSApp terminate:self];
  }
  
  return;
}

////////////////////////////////////////////////////////////////////////////////

- (void)applicationWillTerminate:(NSNotification *)notification
{
  [self terminateTrackpadEventHandling];
}

////////////////////////////////////////////////////////////////////////////////

- (void) receiveSleepNotification: (NSNotification*) notification
{
  [self terminateTrackpadEventHandling];
}

////////////////////////////////////////////////////////////////////////////////

- (void) receiveWakeNotification: (NSNotification*) notification
{
  NSLog(@"receiveWakeNotification: %@", [notification name]);
  if (![self initiateTrackpadEventHandling]) {
    [NSApp terminate:self];
  }
}

////////////////////////////////////////////////////////////////////////////////

- (void) registerSleepWakeNotifications
{
  [[[NSWorkspace sharedWorkspace] notificationCenter]
   addObserver: self
   selector: @selector(receiveSleepNotification:)
   name: NSWorkspaceWillSleepNotification object: NULL];
  
  [[[NSWorkspace sharedWorkspace] notificationCenter]
   addObserver: self
   selector: @selector(receiveWakeNotification:)
   name: NSWorkspaceDidWakeNotification object: NULL];
}

////////////////////////////////////////////////////////////////////////////////

- (BOOL)initiateTrackpadEventHandling
{
  io_iterator_t iterator;
  if (IOServiceGetMatchingServices(kIOMasterPortDefault
                                   , IOServiceMatching(DRIVER_CLASS_NAME_STRING)
                                   , &iterator)
      != KERN_SUCCESS) {
    [self alertMessage:@"Failed to find the service of MbpTrackpadControlKeyDriver.kext."];
    return NO;
  }
  
  connect_ = IO_OBJECT_NULL;
  io_service_t service;
  while ((service = IOIteratorNext(iterator)) != IO_OBJECT_NULL) {
    if (IOServiceOpen(service, mach_task_self(), 0, &connect_) != KERN_SUCCESS) {
      break;
    }
    if (CUserClientFacade().openUserClient(connect_) != KERN_SUCCESS) {
      break;
    }
	}
  IOObjectRelease(iterator);
  
	if (connect_ == IO_OBJECT_NULL) {
    [self alertMessage:@"Failed to open the user client of MbpTrackpadControlKeyDriver.kext."];
    return NO;
	}
  
  if (!_trackpadController->initiate()) {
    [self alertMessage:@"Failed to access the trackpad device."];
    return NO;
  }
  
  return YES;
}

////////////////////////////////////////////////////////////////////////////////

- (void)terminateTrackpadEventHandling
{
  _trackpadController->terminate();
  
  if (connect_ == IO_OBJECT_NULL) {
    return;
	}
	kern_return_t kr0 = CUserClientFacade().closeUserClient(connect_);
  kern_return_t kr1 = IOServiceClose(connect_);
  assert(kr0 == KERN_SUCCESS);
  assert(kr1 == KERN_SUCCESS);
  connect_ = IO_OBJECT_NULL;
}

////////////////////////////////////////////////////////////////////////////////

- (void)handleTrackpadEvent:(BOOL)isPressed
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

////////////////////////////////////////////////////////////////////////////////

- (void)alertMessage:(NSString*)message
{
  NSAlert *alert
  = [NSAlert alertWithMessageText:@"Error"
                    defaultButton:@"Abort"
                  alternateButton:nil
                      otherButton:nil
        informativeTextWithFormat:message];
  [alert runModal];
}

@end
