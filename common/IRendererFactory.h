#ifndef IRENDERER_FACTORY_H
#define IRENDERER_FACTORY_H

namespace Common
{
  class IRenderer;
  class IRendererFactory
  {
  public:
    virtual ~IRendererFactory(){}
    virtual IRenderer *Create() = 0;
  };
}
#endif
