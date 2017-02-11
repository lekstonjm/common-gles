#include <jni.h>

#include "Renderer.h" // NOLINT

#define JNI_METHOD(return_type, method_name) \
  JNIEXPORT return_type JNICALL              \
      Java_com_example_jm_android_1simple_1triangle_MainActivity_##method_name

namespace {

    inline jlong jptr(Renderer *renderer) {
        return reinterpret_cast<intptr_t>(renderer);
    }

    inline Renderer *native(jlong ptr) {
        return reinterpret_cast<Renderer *>(ptr);
    }
}  // anonymous namespace


extern "C" {

JNI_METHOD(jlong, nativeCreateRenderer )
( JNIEnv *
        , jclass
        , jobject
        , jobject) {

return jptr(new Renderer( )) ;
}


JNI_METHOD(void, nativeInitializeGl)
(JNIEnv *, jobject , jlong renderer_handler) {
    native(renderer_handler)->InitializeGl();
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