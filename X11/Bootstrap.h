#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

class Bootstrap
{
private:
  Bootstrap(){};
public:
  virtual ~Bootstrap(){}
  static void Startup();
};

#endif
