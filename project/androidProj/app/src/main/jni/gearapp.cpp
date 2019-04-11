#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#define  LOG_TAG    "gearApp-native"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))

#include <android/native_activity.h>
#include <android_native_app_glue.h>

#include <android/sensor.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <stdlib.h>

#include "../../../../../Source/Scene/scene.h"

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;
    AAssetManager* assetManager;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
};

AAssetManager* globalAssetManager = nullptr;

extern "C" {
	jint JNI_OnLoad(JavaVM* vm, void* reserved);
	jint Java_com_moonfrog_carromgold_MainActivity_mainlib(JNIEnv *, jobject, jobject);
	static int gear_start_app();
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGI("carromgold : JNI_OnLoad executed");
	return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_com_moonfrog_carromgold_MainActivity_mainlib(JNIEnv* env, jobject obj, jobject assetManager)
{
    LOGI("carromgold : mainlib()\n");
    globalAssetManager = AAssetManager_fromJava(env, assetManager);
    if (!globalAssetManager) {
        LOGE("carromgold : AAssetManager_fromJava() returned null.\n");
        assert(false);
    }
	return 0;
}

static int gear_start_app(struct engine* engine) {
    LOGI("carromgold : gear_start_app()\n");
    engine->assetManager = globalAssetManager;
    LOGI("carromgold : width %d, height %d", engine->width, engine->height);
    Scene& gameScene = Scene::GetInstance();
    gameScene.InitScene(engine->width*2, engine->height*2);
    gameScene.Resize(engine->width*2, engine->height*2);
	return 0;
}

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
    // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

    EGLint contextAttrs[] = {
         EGL_CONTEXT_CLIENT_VERSION, 2,
         EGL_NONE
    };
    context = eglCreateContext(display, config, NULL, contextAttrs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    // Initialize GL state.
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    //glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        return;
    }

    //rendering loop
    Scene& gameScene = Scene::GetInstance();
    gameScene.Update();
    gameScene.Render();

    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    if (AKeyEvent_getKeyCode(event) == AKEYCODE_BACK) {
        // actions on back key
        return 0; // <-- return 1 to prevent default handler
    };

    auto *engine = reinterpret_cast<struct engine*>(app->userData);
    Scene& gameScene = Scene::GetInstance();
    int action = AKeyEvent_getAction(event);
    switch (action) {
        case AKEY_EVENT_ACTION_DOWN: {
            gameScene.MouseBtnDown(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
            break;
        }
        case AKEY_EVENT_ACTION_UP: {
            gameScene.MouseBtnUp(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
            break;
        }
    }

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        gameScene.MouseMove(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0));
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                //ENGINE ENTRY POINT
                gear_start_app(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus.
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus.
            break;
    }
}


/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state)
{
    LOGI("carromgold : android_main");
    struct engine engine;

    // Make sure glue isn't stripped.
    //app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    /////////
    int events;
    android_poll_source *pSource;
    do {
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(state, pSource);
            }
        }

        if (state->userData) {
            auto *pengine = reinterpret_cast<struct engine*>(state->userData);
            engine_draw_frame(pengine);
        }
    } while (!state->destroyRequested);

    LOGI("carromgold : EXITED\n");
}
