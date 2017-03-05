#include "Context.h"
#include "IRendererFactory.h"
#define NULL 0L

using namespace Common;


Context *Context::_instance = NULL;

Context *Context::Instance()
{
  if (_instance == NULL)
  {
    _instance = new Context();
  }
  return _instance;
}

void Context::Release()
{
  if (_instance != NULL)
  {
    delete _instance;
  }
}

Context::Context()
{
  _renderer_factory = NULL;
}

Context::~Context()
{
  if (_renderer_factory != NULL)
  {
    delete _renderer_factory;
  }
}

void Context::Register(IRendererFactory *factory)
{
  if (_renderer_factory)
  {
    delete _renderer_factory;
  }
  _renderer_factory = factory;
}


IRendererFactory *Context::GetRendererFactory()
{
  return _renderer_factory;
}
