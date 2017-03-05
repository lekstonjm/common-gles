//
// Created by jm on 05/03/17.
//

#include <triangle/RendererFactory.h>
#include <common/Context.h>
#include "Bootstrap.h"

void Bootstrap::Startup() {
    Common::Context::Instance()->Register(new Triangle::RendererFactory());
}

