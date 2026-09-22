#pragma once

#include "the_device.hpp"
#include "the_buffer.hpp"
#include "the_swapchain.hpp"
#include "the_resources.hpp"
#include "the_render.hpp"
#include "the_frameinfo.hpp"

#include "../../thelogic/headers/keyboard_movement_controller.hpp"
#include "../../thelogic/headers/the_window.hpp"
#include "../../thelogic/headers/the_events.hpp"

#include "../../theloading/headers/the_scene.hpp"

#include "../../thesystems/headers/compute_system.hpp"
#include "../../thesystems/headers/plane_system.hpp"
#include "../../thesystems/headers/opaque_geom.hpp"

#include <vulkan/vulkan_core.h>

namespace the
{
  class TheLoop
  {
    public:
      static constexpr int defWidth = 1920;
      static constexpr int defHeight = 1080;
      static constexpr float defFar = 500.f;
      static constexpr float defNear = 0.1f;

      std::string defShaderPath = "therenderer/shaders/compiled/";

      TheLoop(){};
      ~TheLoop(){};

      TheLoop(const TheLoop&) = delete;
      TheLoop& operator=(const TheLoop&) = delete;

      void render();

    private:

      VkCommandBuffer commandBuffer;
      TheGameObject::Map gameObjects;

      std::vector<std::shared_ptr<TheBuffer>> uboBuffers;

      TheCamera camera{};
      TheWindow theWindow{defWidth, defHeight, "thengine"};
      TheEvents theEvents{theWindow};
      KeyboardMovementController keyboardController{};
      TheDevice theDevice{theWindow};
      TheRender theRenderer{theDevice, theWindow};
      TheScene sceneManager{theDevice, gameObjects, theRenderer};
  };
};
