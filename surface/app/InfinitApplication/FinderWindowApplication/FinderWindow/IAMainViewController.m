//
//  IAMainViewController.m
//  FinderWindowApplication
//
//  Created by infinit on 10/31/12.
//  Copyright (c) 2012 infinit. All rights reserved.
//

#import "IAMainViewController.h"
#import "IADropFileView.h"
#import "IAUserSearchBar.h"
#import "IAGapState.h"

@interface IAMainViewController ()
{
    BOOL _locked;
}

@property(retain) IBOutlet IADropFileView* drop_box;
@property(retain) IBOutlet IAUserSearchBar* search_bar;
@property(retain) IBOutlet NSButton* send_button;

-(IBAction) sendFile:(id)sender;

@end

@implementation IAMainViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        _locked = false;
    }
    
    return self;
}

- (void)awakeFromNib
{
    [self.search_bar setFocusRingType:NSFocusRingTypeNone];
    [self refresh];
}

-(IBAction) sendFile:(id)sender
{
    _locked = true;
    [self refresh];
    [[IAGapState instance] sendFiles:self.drop_box.pending_files
                              toUser:[self.search_bar getUser]
                     performSelector:@selector(_onFileSent:)
                            onObject:self];
}

- (void) _onFileSent:(IAGapOperationResult*)result
{
    NSLog(@"FILE SENT: %d", result.success);
}

- (void)refresh
{
    if (!_locked && [[self drop_box] hasPendingFiles])
    {
        [[self.search_bar cell] setPlaceholderString:@"Enter an email address"];
        [self.search_bar setEnabled:true];
        if ([self.search_bar isValid])
        {
            [self.send_button setEnabled:true];
        }
        else
        {
            [self.send_button setEnabled:false];
        }
    }
    else
    {
        [[self.search_bar cell] setPlaceholderString:@"Drop files to share!"];
        [self.search_bar setEnabled:false];
        [self.send_button setEnabled:false];
    }
}

@end