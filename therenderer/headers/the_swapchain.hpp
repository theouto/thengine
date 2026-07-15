#pragma once

#include "the_device.hpp"

#include <vector>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace the
{

  class TheSwapChain
  {
    public:

      static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
      TheSwapChain(TheDevice &deviceRef, VkExtent2D windowExtent);
      TheSwapChain(TheDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<TheSwapChain> previous);
      ~TheSwapChain();

      TheSwapChain(const TheSwapChain &) = delete;
      TheSwapChain &operator=(const TheSwapChain&) = delete;

      /*
      I will create the resources as needed and then simply return the index of that resource, which will then
      be used to retreive the related resources as needed. Still not sure as to whether or not I want to do it
      this way or if I want to return a pair with the index and the resource.
      */

      uint32_t createDepthImage();
      uint32_t createColorImage();
      uint32_t createFrameBuffer();
      uint32_t createRenderPass();
      uint32_t createImageView();

      uint32_t width() { return swapChainExtent.width; }
      uint32_t height() { return swapChainExtent.height; }

      //images are nxn anyways
      uint32_t shadowRes() {return shadowExtent.height;}
      void setShadowRes(uint32_t sq) {shadowExtent = {sq, sq};}

      VkRenderPass getRenderPass(int index) {return renderPasses[index];}
      VkImageView getImageView(int index) {return imageViews[index];}
      VkImageView getDepthView(int index) {return depthImageViews[index];}
      VkFramebuffer getFramebuffer(int index) {return framebuffers[index];}

      float extentAspectRatio() 
      {
          return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
      }

      VkFormat findDepthFormat();

      VkResult acquireNextImage(uint32_t *imageIndex);
      VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

      bool compareSwapFormats(const TheSwapChain& swapChain) const
      {
          return swapChain.swapChainDepthFormat == swapChainDepthFormat && swapChain.swapChainImageFormat == swapChainImageFormat;
      }

    private:

      void init();
      void createSyncObjects();
      void createSwapChain();

      VkFormat swapChainImageFormat;
      VkFormat swapChainDepthFormat;

      //I should maybe move the swapchainextent to also be a vector, in case I want to render certain pipelines at
      //lower resolutions more easily
      VkExtent2D swapChainExtent; // = {TheRenderer::defWidth, TheRenderer::defHeight};

      //just multiply as needed
      VkExtent2D shadowExtent = {1024, 1024};

      /*
      Learning from how I worked with this on the previous vulkan renderer that I wrote, I will simply
      create images as needed for the different pipelines, creating the required variables as needed,
      and then passing over the reference to the variable on the vector (or and index, this is still to be decided).

      This will help me keep a clean swapchain file, because if you see the one belonging to the previous project,
      it's a mess, and horrible to scale for when the renderer inevitably crosses over the 50 pipeline mark, for example.

      I am also taking the time to comment my code, because I do want this project to be better than the previous one.
      */

      std::vector<VkFramebuffer> framebuffers;
      std::vector<VkImage> depthImages;
      std::vector<VkDeviceMemory> depthImageMemorys;
      std::vector<VkImageView> depthImageViews;
      std::vector<VkImage> images;
      std::vector<VkImageView> imageViews;
      std::vector<VkRenderPass> renderPasses;

      TheDevice& device;

      VkSwapchainKHR swapChain;
      std::shared_ptr<TheSwapChain> oldSwapChain;

      std::vector<VkSemaphore> imageAvailableSemaphores;
      std::vector<VkSemaphore> renderFinishedSemaphores;
      std::vector<VkFence> inFlightFences;
      std::vector<VkFence> imagesInFlight;
      size_t currentFrame = 0;
  };
}
