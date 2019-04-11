//
//  OSUtils.h
//  GDCPhysics
//
//  Created by Arun A on 03/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#include <string>

namespace OSUtils {
#if ANDROID
    static std::string writablePath;
    void cpp_setWritablePath(const std::string& path);
    const std::string& cpp_getWritablePath();
#endif
    const std::string cpp_getCurrentWorkingDirectory();
    std::string cpp_getPath(const std::string& path);
}


