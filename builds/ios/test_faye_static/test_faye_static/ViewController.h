//
//  ViewController.h
//  test_faye_static
//
//  Created by Resident evil on 2/17/15.
//  Copyright (c) 2015 Resident evil. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController

@property (nonatomic, weak) IBOutlet UITextField * textField;

- (IBAction) onSendText:(id)sender;
- (IBAction) onCreate:(id)sender;
- (IBAction) onConnect:(id)sender;



@end

