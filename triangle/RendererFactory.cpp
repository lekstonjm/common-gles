#include "RendererFactory.h"
#include "Renderer.h"

using namespace Triangle;

Common::IRenderer *RendererFactory::Create()
{
  return new Renderer();
}
