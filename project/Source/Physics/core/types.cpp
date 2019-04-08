//
//  types.cpp
//  waves2
//
//  Created by arun on 26/05/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "types.h"
#include <string>

static char g_cszResourcePath[128];
static char g_cszCommonResourcePath[128];
static char g_cszReturnPath1[256];
static char g_cszReturnPath2[256];

void setResourcePath(const char* path)
{
    strcpy(g_cszResourcePath, path);
}

void setCommonResourcePath(const char* path)
{
    strcpy(g_cszCommonResourcePath, path);
}

const char* getResourcePath(const char* relativepath)
{
    if(relativepath==NULL)
        return g_cszResourcePath;
    
    sprintf(g_cszReturnPath1, "%s%s", g_cszResourcePath, relativepath);
    return g_cszReturnPath1;
}

const char* getCommonResourcePath(const char* relativepath)
{
    if(relativepath==NULL)
        return g_cszCommonResourcePath;
    
    sprintf(g_cszReturnPath1, "%s%s", g_cszCommonResourcePath, relativepath);
    return g_cszReturnPath1;
}

const char* appendPath(const char* path1, const char* path2)
{
    sprintf(g_cszReturnPath2, "%s%s", path1, path2);
    return g_cszReturnPath2;
}

int getRandom(int start, int end) {
    if (end<=start) {
        return start;
    }
    return start + rand()%(end-start);
}
