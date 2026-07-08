#pragma once

#include <SDL3/SDL.h>
#include<SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <string>

namespace the
{
  class TheWindow
  {
    public:

      TheWindow(int w, int h, std::string name);
      ~TheWindow();

      VkExtent2D getExtent() {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}
      SDL_Window* getSDLWindow() const {return window;}
      void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
      bool eventWatcher();

    private:
      void initWindow();

	  int width;
	  int height;
	  bool framebufferResized = false;

	  std::string windowName;
	  SDL_Window* window;
  };
}
