#APP_STL := stlport_static
APP_STL := c++_shared
APP_CFLAGS := -DDEBUG -DASIO_STANDALONE -fexceptions -frtti -DGEAR_ANDROID=1 -DUSE_ProgrammablePipeLine=1 -DUSE_OPENSL=1 -DLOG_DEBUG_ENGINE=1