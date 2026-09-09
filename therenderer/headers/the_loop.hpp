#pragma once

#include "the_device.hpp"
#include "the_swapchain.hpp"
#include "the_resources.hpp"
#include "the_render.hpp"
#include "the_frameinfo.hpp"

#include "../../thelogic/headers/keyboard_movement_controller.hpp"
#include "../../thelogic/headers/the_window.hpp"
#include "../../thelogic/headers/the_events.hpp"

#include "../../theloading/headers/the_scene.hpp"

namespace the
{
  class TheLoop
  {
    public:
      static constexpr int defWidth = 1920;
      static constexpr int defHeight = 1080;

      TheLoop();
      ~TheLoop(){};

      void render();

    private:
      TheWindow theWindow{defWidth, defHeight, "thengine"};
      TheDevice theDevice{theWindow};
  };
};
