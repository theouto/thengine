#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <string>

namespace the
{
  class TheWindow
  {
    public:

      TheWindow(int w, int h, std::string name);
      ~TheWindow();

      bool shouldClose() { return false; }
	  VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
	  bool wasWindowResized() { return framebufferResized; };
	  void resetWindowResizedFlag() { framebufferResized = false; };
      void resize() {framebufferResized = true; SDL_GetWindowSize(window, &width, &height);}
	  SDL_Window *getSDLwindow() const { return window; }

	  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
      void initWindow();

	  int width;
	  int height;
	  bool framebufferResized = false;

	  std::string windowName;
	  SDL_Window* window;
  };
}
