#ifndef RENDERER_FACTORY_H
#define RENDERER_FACTORY_H

#include <IRendererFactory.h>

namespace Triangle
{
  class RendererFactory : public Common::IRendererFactory
  {
  public:
    virtual Common::IRenderer *Create();
  };
}
#endif
