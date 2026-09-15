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

#include "../../thesystems/headers/compute_system.hpp"

#include <vulkan/vulkan_core.h>

namespace the
{
  class TheLoop
  {
    public:
      static constexpr int defWidth = 1920;
      static constexpr int defHeight = 1080;

      TheLoop(){};
      ~TheLoop(){};

      TheLoop(const TheLoop&) = delete;
      TheLoop& operator=(const TheLoop&) = delete;

      void render();

    private:

      TheCamera camera{};
      TheWindow theWindow{defWidth, defHeight, "thengine"};
      TheEvents theEvents{theWindow};
      KeyboardMovementController keyboardController{};
      TheDevice theDevice{theWindow};
      TheRender theRenderer{theDevice, theWindow};
      TheScene sceneManager{theDevice, gameObjects, theRenderer};

      VkCommandBuffer commandBuffer;
      TheGameObject::Map gameObjects;
  };
};
