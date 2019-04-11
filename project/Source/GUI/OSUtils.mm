//
//  OSUtils.m
//  GDCPhysics
//
//  Created by Arun A on 03/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "OSUtils.h"

namespace OSUtils {
const std::string cpp_getCurrentWorkingDirectory() {
    NSString *appPath = [[NSBundle mainBundle] bundlePath];
    const char* cString = [appPath cStringUsingEncoding:NSASCIIStringEncoding];
    return std::string((cString)?cString:"");
}

std::string cpp_getPath(const std::string& path) {
    std::string resolvedPath = "./Contents/Resources/"+path;
    return resolvedPath.c_str();
}
};
