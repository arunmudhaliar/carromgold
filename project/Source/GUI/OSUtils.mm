//
//  OSUtils.m
//  GDCPhysics
//
//  Created by Arun A on 03/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "OSUtils.h"

//+(const char*) getCurrentWorkingDirectory:(id)sender
//{
//    NSString *appPath = [[NSBundle mainBundle] bundlePath];
//    const char* cString = [appPath cStringUsingEncoding:NSASCIIStringEncoding];
//    
//    return cString;
//}

const char* cpp_getCurrentWorkingDirectory()
{
//    return [OSSpecificImpl getCurrentWorkingDirectory:nullptr];
    NSString *appPath = [[NSBundle mainBundle] bundlePath];
    const char* cString = [appPath cStringUsingEncoding:NSASCIIStringEncoding];
    
    return cString;
}

std::string cpp_getPath(const std::string& path) {
    std::string resolvedPath = "./Contents/Resources/"+path;
    return resolvedPath.c_str();
}
