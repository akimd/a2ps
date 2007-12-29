
#import "AppDelegate.h"

#import <foundation/NSArray.h>
#import <dbkit/DBValue.h>

@implementation AppDelegate

// --------------------- Constants

#define N_FIELDS		(8)

// --------------------- Global variables

NSString
    *AppDelegateNewSetNotificationName = @"AppDelegateNewSetNotification",
    	// This is the name of the notification sent out when a new
	// field set has been selected for manipulation.
    *AppDelegateNewFieldNotificationName = @"AppDelegateNewSetNotification";
    	// This is the name of the notification sent out when a new
	// field has been selected for manipulation.
    
// --------------------- Class variables

static NSMutableArray
	*classDefaultSet = nil;
	
// --------------------- Methods

+ initize
{
    // This method will be called automatically prior to any
    // other to this class.
    
    int 
    	i;
	
    // Create an array to hold the actual field objects.
    classDefaultSet = [[NSMutableArray array] retain];
    		    
        
    return self;
    		    
} // initize
    
- (id <FieldSet>)fieldSet
{
    // This method returns an object (conforming to the FieldSet
    // protocol) which is being manipulated by this application.  
    
    // If we don't have one yet...
    if (!myFieldSet)
    {
    	// Just adopt the default one for this class.
	[self setFieldSet:(id)classDefaultSet];
    }
    
    return myFieldSet;
    
} // fieldSet

- (void)setFieldSet:(id <FieldSet>)newValue
{
    // This method sets the field set being manipulated to the one given.
    // If the new set differs from the old, the AppDelegateNewSet
    // notification will be posted informing watchers of the change.
    
    // If the new field is the same as the old..
    if (newValue EQ myFieldSet)
    {
    	return;	// do nothing
    } 
    
    // Release the old value and adopt the new.
    [newValue retain];
    
    // Post a notification of the change.
    [[NotificationCenter defaultCenter]
    	postNotificationName:AppDelegateNewSetNotificationName
	object:myFieldSet];
    
} // setFieldSet:

- (Field *)selectedField
{
    // This method returns the field from our field set
    // which is currently considered selected.
    
    // If we don't currently have a selected field...
    if (!mySelectedField)
    {
     	// Adopt the first field in our set.
	[self setSelectedField:[[self fieldSet] fieldAtIndex:0]];
    }
    
    return mySelectedField;
    
} // selectedField

- (void)setSelectedField:(Field *)newValue
{
    // This method causes the given field to be taken as the active one.
    // If the new field differs from the old, the AppDelegateNewField 
    // notification will be posted informing watchers of the change.
    
    // If the new field is the same as the old..
    if (newValue EQ mySelectedField)
    {
    	return;	// do nothing
    } 
    
    // Release the old value and adopt the new.
    [newValue retain];
    
    // Post a notification of the change.
    [[NotificationCenter defaultCenter]
    	postNotificationName:AppDelegateNewFieldNotificationName 
	object:mySelectedField];
    
} // setSelectedField:

@end
