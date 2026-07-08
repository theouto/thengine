#include "../headers/the_window.hpp"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#include <stdexcept>

namespace the
{
	TheWindow::TheWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name }
	{
		initWindow();
	}

	TheWindow::~TheWindow()
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void TheWindow::initWindow()
	{
	  SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	  window = SDL_CreateWindow(windowName.c_str(), width, height, 
                                SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

      //make the path nice
      SDL_Surface* image = IMG_Load("textures/NEEERDDDD.png");
	  SDL_SetWindowIcon(window, image);
	}

    void TheWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
	  if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, surface))
	  {
	    throw std::runtime_error("failed to create window surface");
	  }
	}
	
    void TheWindow::resize()
    {
	  framebufferResized = true;
      SDL_GetWindowSize(window, &width, &height);
    }
}
