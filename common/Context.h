#ifndef CONTEXT_H
#define CONTEXT_H

namespace Common
{
  class IRendererFactory;
  class Context
  {
  private:
    static Context *_instance;
    IRendererFactory *_renderer_factory;
    Context();
  public:
    virtual ~Context();
    static Context *Instance();
    static void Release();
    void Register(IRendererFactory *factory);
    IRendererFactory *GetRendererFactory();
  };
}
#endif
