#pragma once

#include "the_device.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace the
{

  class TheSwapChain
  {
    public:

      static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

      enum PipelineSettings
      {
        MAIN_COMP,
        MAIN_GEOM,
        OTHER,
        SINGULAR,
        SYNCED,
        NFRAMES,
        COLOR,
        DEPTH,
        ADDITIONAL_DEPTH,
        NO_ADDITIONAL_DEPTH,
      };

      TheSwapChain(TheDevice &deviceRef, VkExtent2D windowExtent);
      TheSwapChain(TheDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<TheSwapChain> previous);
      ~TheSwapChain();

      TheSwapChain(const TheSwapChain &) = delete;
      TheSwapChain &operator=(const TheSwapChain&) = delete;

      std::vector<uint32_t> createNeededResources(PipelineSettings pass, 
                                                  PipelineSettings frameNumber, 
                                                  PipelineSettings color,
                                                  PipelineSettings depth,
                                                  VkExtent2D resolution,
                                                  uint32_t frames = 0);

      void setImageExtent(uint32_t index, uint32_t w, uint32_t h) {extents[index] = {w, h};}

      VkRenderPass getRenderPass(uint32_t index) {return renderPasses[index];}
      VkImageView getImageView(uint32_t index) {return imageViews[index];}
      VkFramebuffer getFrameBuffer(uint32_t index) {return framebuffers[index];}
      VkExtent2D getImageExtent(uint32_t index) {return extents[index];}
      float getImageAspectRatio(uint32_t index){return static_cast<float>(extents[index].width)/static_cast<float>(extents[index].height);}

      VkFormat findDepthFormat();
      VkFormat getSwapChainDepthFormat(){return swapChainDepthFormat;}
      VkFormat getSwapChainImageFormat(){return swapChainImageFormat;}

      bool isSynced(uint32_t index){return synced[index];}

      VkResult acquireNextImage(uint32_t *imageIndex);
      VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);
      int getImageViewCount(){return imageViewCount;}

      bool compareSwapFormats(const TheSwapChain& swapChain) const
      {
          return swapChain.swapChainDepthFormat == swapChainDepthFormat && swapChain.swapChainImageFormat == swapChainImageFormat;
      }

    private:

      void init();
      void createSyncObjects();
      void createSwapChain();

      // Helper functions
      VkSurfaceFormatKHR chooseSwapSurfaceFormat(
          const std::vector<VkSurfaceFormatKHR> &availableFormats);
      VkPresentModeKHR chooseSwapPresentMode(
          const std::vector<VkPresentModeKHR> &availablePresentModes);
      VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

      /*
      I will create the resources as needed and then simply return the index of that resource, which will then
      be used to retreive the related resources as needed. Still not sure as to whether or not I want to do it
      this way or if I want to return a pair with the index and the resource.
      */
      uint32_t createImage(PipelineSettings setting);
      uint32_t createFrameBuffer(uint32_t imageIndex, uint32_t pipelineIndex, bool initoverride = false);
      uint32_t createRenderPass(PipelineSettings depth);
      void createImageView(uint32_t workingViewIndex, uint32_t workingIndex, VkFormat format);

      VkFormat swapChainImageFormat;
      VkFormat swapChainDepthFormat;

      /*
      Learning from how I worked with this on the previous vulkan renderer that I wrote, I will simply
      create images as needed for the different pipelines, creating the required variables as needed,
      and then passing over the reference to the variable on the vector (or and index, this is still to be decided).

      This will help me keep a clean swapchain file, because if you see the one belonging to the previous project,
      it's a mess, and horrible to scale for when the renderer inevitably crosses over the 50 pipeline mark, for example.

      I am also taking the time to comment my code, because I do want this project to be better than the previous one.
      */

      std::vector<VkImage> placeholderImages;

      std::unordered_map<uint32_t, VkFramebuffer> framebuffers;
      std::unordered_map<uint32_t, VkDeviceMemory> imageMemorys;
      std::unordered_map<uint32_t, VkImage> images;
      int imageViewCount = 0;
      std::unordered_map<uint32_t, VkImageView> imageViews;
      std::unordered_map<uint32_t, VkRenderPass> renderPasses;
      std::unordered_map<uint32_t, VkExtent2D> extents;

      std::unordered_map<uint32_t, bool> synced;
      std::unordered_map<uint32_t, bool> addedDepth;

      VkExtent2D swapChainExtent;
      VkExtent2D windowExtent;

      uint32_t currentIndex = 0;
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
