LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := carromgold
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lOpenSLES
LOCAL_STATIC_LIBRARIES := android_native_app_glue
#$(call import-add-path,$(LOCAL_PATH)/websocketpp-master)
#$(call import-add-path,$(LOCAL_PATH)/asiosdk)
LOCAL_SRC_FILES := gearapp.cpp  \
                    ../../../../../Source/Physics/colliders/boxCollider.cpp \
                    ../../../../../Source/Physics/core/types.cpp \
                    ../../../../../Source/Physics/core/Timer.cpp \
                    ../../../../../Source/Physics/rigidBody.cpp \
                    ../../../../../Source/Physics/solver.cpp \
                    ../../../../../Source/Scene/ball.cpp \
                    ../../../../../Source/Scene/gxRenderer.cpp \
                    ../../../../../Source/Scene/hole.cpp \
                    ../../../../../Source/Scene/input.cpp \
                    ../../../../../Source/Scene/scene.cpp \
                    ../../../../../Source/Scene/board.cpp \
                    ../../../../../Source/Scene/stricker.cpp \
                    ../../../../../Source/Scene/wall.cpp \
                    ../../../../../Source/GUI/hwShader/gxHWShader.cpp \
                    ../../../../../Source/GUI/hwShader/HWShaderManager.cpp \
                    ../../../../../Source/GUI/Sprite/geTextureManager.cpp \
                    ../../../../../Source/GUI/Sprite/Sprite.cpp \
                    ../../../../../Source/GUI/Sprite/Sprite2Dx.cpp \
                    ../../../../../Source/GUI/geFontManager.cpp \
                    ../../../../../Source/GUI/gxDebug.cpp \
                    ../../../../../Source/GUI/gxFile.cpp \
                    ../../../../../Source/GUI/gxBufferFileReader.cpp \
                    ../../../../../Source/GUI/OSUtils.cpp \
                    ../../../../../Source/GUI/gxButton.cpp \
                    ../../../../../Source/audio/SoundSample.cpp \
                    ../../../../../Source/audio/SoundSource.cpp \
                    ../../../../../Source/audio/SoundEngine.cpp \
                    ../../../../../Source/zlib-1.2.11/adler32.c	\
                    ../../../../../Source/zlib-1.2.11/compress.c	\
                    ../../../../../Source/zlib-1.2.11/crc32.c	\
                    ../../../../../Source/zlib-1.2.11/deflate.c	\
                    ../../../../../Source/zlib-1.2.11/infback.c	\
                    ../../../../../Source/zlib-1.2.11/inffast.c	\
                    ../../../../../Source/zlib-1.2.11/inflate.c	\
                    ../../../../../Source/zlib-1.2.11/inftrees.c	\
                    ../../../../../Source/zlib-1.2.11/trees.c	\
                    ../../../../../Source/zlib-1.2.11/uncompr.c	\
                    ../../../../../Source/zlib-1.2.11/zutil.c    \
                    ../../../../../Source/libpng16/png.c    \
                    ../../../../../Source/libpng16/pngerror.c    \
                    ../../../../../Source/libpng16/pngget.c    \
                    ../../../../../Source/libpng16/pngmem.c    \
                    ../../../../../Source/libpng16/pngpread.c    \
                    ../../../../../Source/libpng16/pngread.c    \
                    ../../../../../Source/libpng16/pngrio.c    \
                    ../../../../../Source/libpng16/pngrtran.c    \
                    ../../../../../Source/libpng16/pngrutil.c    \
                    ../../../../../Source/libpng16/pngset.c    \
                    ../../../../../Source/libpng16/pngtrans.c    \
                    ../../../../../Source/libpng16/pngwio.c    \
                    ../../../../../Source/libpng16/pngwrite.c    \
                    ../../../../../Source/libpng16/pngwtran.c    \
                    ../../../../../Source/libpng16/pngwutil.c    \

#LOCAL_C_INCLUDES := $(LOCAL_PATH)/websocketpp-master
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/asiosdk
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libpng16
LOCAL_C_INCLUDES += $(LOCAL_PATH)/zlib-1.2.11

APP_OPTIM := debug
include $(BUILD_SHARED_LIBRARY)
$(call import-module, android/native_app_glue)