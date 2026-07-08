#pragma once

#include "the_camera.hpp"
#include "the_device.hpp"
#include "the_swapchain.hpp"

namespace the
{
  class TheRender
  {
    public:
      static constexpr int defWidth = 1920;
      static constexpr int defHeight = 1080;
    private:

      TheWindow theWindow{defWidth, defHeight, "thengine"};
      TheDevice theDevice{theWindow};
  };
}
