#ifndef IRENDERER_H
#define IRENDERER_H

namespace Common
{
  class IRenderer
  {
  public:
    virtual ~IRenderer(){};
    virtual void InitializeGl()=0;
    virtual void ReleaseGl()=0;
    virtual void SetViewport(int width, int height)=0;
    virtual void DrawFrame()=0;
  };
}

#endif
