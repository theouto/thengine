#include "../headers/the_window.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <stb_image.h>

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

		//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = SDL_CreateWindow(windowName.c_str(), width, height, 
                                 SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

        //the nerd will sing again at a later date.
		SDL_Surface image;
		image.pixels = stbi_load("textures/NEEERDDDD.png", &image.w, &image.h, 0, 4); //rgba channels 
	    SDL_SetWindowIcon(window, &image);
		stbi_image_free(image.pixels);

		//glfwSetWindowUserPointer(window, this);
		//glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void TheWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (SDL_Vulkan_CreateSurface(window, instance, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

    /*
	void TheWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto lveWindow = reinterpret_cast<TheWindow*>(glfwGetWindowUserPointer(window));
		lveWindow->framebufferResized = true;
		lveWindow->width = width;
		lveWindow->height = height;
	}
    */
}
