//
//  AppDelegate.m
//  GDCPhysics
//
//  Created by Arun A on 30/10/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#import "AppDelegate.h"
#include "../Source/appEntry.h"
#include "../Source/Scene/scene.h"
@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    GDCPhysics gdcPhysics;
    gdcPhysics.appEntry();
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

-(IBAction) resetSimulation:(id)sender {
    Scene::GetInstance().StartGameFromMenu();
}
@end
