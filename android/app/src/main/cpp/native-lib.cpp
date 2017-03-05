#include <jni.h>
#include <cstdint>
#include "Bootstrap.h"
#include <Context.h>
#include <IRendererFactory.h>
#include <IRenderer.h>

#define JNI_METHOD(return_type, method_name) \
  JNIEXPORT return_type JNICALL              \
      Java_com_example_jm_android_1simple_1triangle_MainActivity_##method_name

namespace {

    inline jlong jptr(Common::IRenderer *renderer) {
        return reinterpret_cast<intptr_t>(renderer);
    }

    inline Common::IRenderer *native(jlong ptr) {
        return reinterpret_cast<Common::IRenderer *>(ptr);
    }
}  // anonymous namespace


extern "C" {

JNI_METHOD(void, nativeStartup)
(JNIEnv *, jobject ) {
    Bootstrap::Startup();
}

JNI_METHOD(jlong, nativeCreateRenderer )
( JNIEnv *, jobject) {

return jptr(Common::Context::Instance()->GetRendererFactory()->Create()) ;
}


JNI_METHOD(void, nativeInitializeGl)
(JNIEnv *, jobject , jlong renderer_handler) {
    native(renderer_handler)->InitializeGl();
}

JNI_METHOD(void, nativeSetViewport)
(JNIEnv *, jclass , jlong renderer_handler, int width, int height) {
    native(renderer_handler)->SetViewport(width, height);
}

JNI_METHOD(void, nativeDrawFrame)
(JNIEnv *, jobject , jlong renderer_handler) {
    native(renderer_handler)->DrawFrame();
}

JNI_METHOD(void, nativeDestroyRenderer)
(JNIEnv *, jclass , jlong renderer_handler) {
    native(renderer_handler)->ReleaseGl();
    delete native(renderer_handler);
}

}