

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;

@property (assign) IBOutlet NSTextField * textField;

- (IBAction) onCreate:(id)sender;
- (IBAction) onConnect:(id)sender;
- (IBAction) onSendText:(id)sender;

@end
