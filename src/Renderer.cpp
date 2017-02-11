//
// Created by jm on 29/01/17.
//
#include <math.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer.h"

void Renderer::InitializeGl()
{
    _start = std::chrono::system_clock::now();

    GLfloat vertices_colors[] = {
            -0.5f, -0.5, 0.0f
            ,  1.0f, 1.0f, 0.0f
            ,  0.5f, -0.5f, 0.0f
            ,  1.0f, 0.0f, 0.0f
            ,  0.0f, 0.5f, 0.0f
            ,  0.0f, 1.0f, 0.0f
    };
    glGenBuffers(1,&_triangle_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _triangle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_colors) ,vertices_colors, GL_STATIC_DRAW);

    const char* const fragment_source = R"glsl(
        varying lowp vec3 linear_color;
        uniform lowp float fade;
        void main(void)
        {
          gl_FragColor = vec4(linear_color.x,linear_color.y,linear_color.z,fade);
        }
      )glsl";
    _fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(_fragment_shader, 1, (const char**)&fragment_source, NULL);
    glCompileShader(_fragment_shader);

    const char* const vertex_source =R"glsl(
      attribute highp vec4 vertex;
      attribute lowp vec3 color;
      varying lowp vec3 linear_color;
      uniform mediump mat4 pmv_matrix;
      void main()
      {
        gl_Position = pmv_matrix * vertex;
        linear_color = color;
      }
    )glsl";
    _vertex_shader= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(_vertex_shader, 1, (const char**)&vertex_source, NULL);
    glCompileShader(_vertex_shader);

    _program_shader = glCreateProgram();
    glAttachShader(_program_shader, _fragment_shader);
    glAttachShader(_program_shader, _vertex_shader);
    glLinkProgram(_program_shader);

    _vertex_location = glGetAttribLocation(_program_shader, "vertex");
    _color_location = glGetAttribLocation(_program_shader,"color");
    _pmv_matrix_location = glGetUniformLocation(_program_shader, "pmv_matrix");
    _fade_location = glGetUniformLocation(_program_shader, "fade");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::SetViewport(int width, int height)
{
  glViewport(0,0,width,height);
  _ratio = (float) width / height;
  std::cout<<width<<" "<<height<<" "<<_ratio<<std::endl;
}

void Renderer::DrawFrame() {
    std::chrono::duration< double, std::ratio<1l> > duration = std::chrono::system_clock::now() - _start;
    float fade = (float)cos( duration.count() * 2.0 * 3.141592 * 1/10.0);
    fade *= fade;
    glClearColor(0.2,0.2,0.2,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glm::mat4 pvm_matrix = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    //glm::mat4 view =  glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,-1.0f));

    glm::mat4 projection = glm::ortho(-_ratio, _ratio , -1.0f, 1.0f, -1.0f, 1.0f);
    pvm_matrix = projection * view * model;
/*
    glm::mat4 Proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 10.f);
    glm::mat4 View = glm::mat4();
    glm::mat4 Model = glm::mat4(1.0f);

    GLfloat identity_matrix[] =
            {
                    1,0,0,0
                    , 0,1,0,0
                    , 0,0,1,0
                    , 0,0,0,1
            };
*/
    glUseProgram(_program_shader);
    glUniformMatrix4fv(_pmv_matrix_location, 1, GL_FALSE, glm::value_ptr(pvm_matrix) );
    glUniform1f(_fade_location,fade);
    glEnableVertexAttribArray(_vertex_location);
    glEnableVertexAttribArray(_color_location);
    glBindBuffer(GL_ARRAY_BUFFER, _triangle_vbo);
    glVertexAttribPointer(_vertex_location, 3, GL_FLOAT, GL_FALSE
            , (3+3)*sizeof(GLfloat)
            , 0);
    glVertexAttribPointer(_color_location, 3, GL_FLOAT, GL_FALSE
            , (3+3)*sizeof(GLfloat)
            , (GLvoid*)(3*sizeof(GLfloat))
    );
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(_vertex_location);
    glDisableVertexAttribArray(_color_location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::ReleaseGl()  {
    glDeleteShader(_program_shader);
    glDeleteShader(_vertex_shader);
    glDeleteShader(_fragment_shader);
}
