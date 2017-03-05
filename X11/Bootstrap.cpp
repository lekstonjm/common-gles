#include "Bootstrap.h"
#include <common/Context.h>
#include <triangle/RendererFactory.h>

void Bootstrap::Startup()
{
  Common::Context::Instance()->Register(new Triangle::RendererFactory());
}
