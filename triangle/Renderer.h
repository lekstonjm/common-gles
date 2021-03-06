//
// Created by jm on 29/01/17.
//

#ifndef ANDROID_SIMPLE_TRIANGLE_RENDERER_H
#define ANDROID_SIMPLE_TRIANGLE_RENDERER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <chrono>
#include <IRenderer.h>

namespace Triangle
{
  class Renderer : public Common::IRenderer {
  //
  // Created by jm on 29/01/17.
  //

  public:
      Renderer(){}
      ~Renderer(){};
      void InitializeGl();
      void ReleaseGl();
      void SetViewport(int width, int height);
      void DrawFrame();
  private:
      GLuint _vertex_shader;
      GLuint _fragment_shader;
      GLuint _program_shader;
      GLuint _triangle_vbo;
      GLint _vertex_location;
      GLint _color_location;
      GLint _fade_location;
      GLint _pmv_matrix_location;
      std::chrono::time_point<std::chrono::system_clock> _start;
      GLfloat _ratio;
  };
}

#endif //ANDROID_SIMPLE_TRIANGLE_RENDERER_H
