#include "ApplicationDelegate.hh"

@implementation ApplicationDelegate {

}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];

    // Create application menu
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSMenu *menuBar = [[NSMenu alloc] initWithTitle:@""];
    [NSApp setMainMenu:menuBar];

    NSMenuItem *item;
    NSMenu *menu;

    item = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [menuBar addItem:item];

    menu = [[NSMenu alloc] initWithTitle:@"Apple"];
    [menuBar setSubmenu:menu forItem:item];
    [item release];

    item = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    [menu addItem:item];
    [item release];

    [menu release];
    [menuBar release];
    [pool release];
}

@end
