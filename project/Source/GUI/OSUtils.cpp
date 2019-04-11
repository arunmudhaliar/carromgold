//
//  OSUtils.m
//  GDCPhysics
//
//  Created by Arun A on 03/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#include "OSUtils.h"

namespace OSUtils {
    const std::string cpp_getCurrentWorkingDirectory() {
        return "";
    }

    void cpp_setWritablePath(const std::string& path) {
        writablePath = path;
    }

    const std::string& cpp_getWritablePath() {
        return writablePath;
    }

    void cpp_setAssetManager(AAssetManager* am) {
        assetManager = am;
    }

    AAssetManager* cpp_getAssetManager() {
        return assetManager;
    }

    std::string cpp_getPath(const std::string &path) {
        return /*writablePath +*/ path;
    }
}