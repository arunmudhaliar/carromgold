//
//  OSUtils.h
//  GDCPhysics
//
//  Created by Arun A on 03/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#include <string>

#if ANDROID
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
#endif

namespace OSUtils {
#if ANDROID
    static std::string writablePath;
    static AAssetManager* assetManager;
    void cpp_setWritablePath(const std::string& path);
    const std::string& cpp_getWritablePath();
    void cpp_setAssetManager(AAssetManager*);
    AAssetManager* cpp_getAssetManager();
#endif
    const std::string cpp_getCurrentWorkingDirectory();
    std::string cpp_getPath(const std::string& path);
}


