

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController

@property (nonatomic, weak) IBOutlet UITextField * textField;

- (IBAction) onSendText:(id)sender;
- (IBAction) onCreate:(id)sender;
- (IBAction) onConnect:(id)sender;



@end

