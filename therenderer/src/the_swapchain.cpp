#include "../headers/the_swapchain.hpp"

#include <iostream>
#include <cassert>
#include <vulkan/vulkan_core.h>

namespace the
{
  TheSwapChain::TheSwapChain(TheDevice &deviceRef, VkExtent2D windowExtent) : device{deviceRef}, windowExtent{windowExtent}
  {
    createSwapChain();
  }

  TheSwapChain::TheSwapChain(TheDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<TheSwapChain> previous) : device{deviceRef}, windowExtent{windowExtent}
  {
    createSwapChain();

    

    previous = nullptr;
  }

  void TheSwapChain::createSwapChain()
  {
    SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = device.surface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device.findPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = oldSwapChain == nullptr ? VK_NULL_HANDLE : oldSwapChain->swapChain;

    if (vkCreateSwapchainKHR(device.device(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
    }

    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, nullptr);
    placeholderImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, placeholderImages.data());
 
    for (int i = 0; i < imageCount; i++)
    {
      images[i] = placeholderImages[i];
    }

    swapChainImageFormat= surfaceFormat.format;
    swapChainExtent = extent;
  }

  std::vector<uint32_t> TheSwapChain::createNeededResources(PipelineSettings pass, 
                                                            PipelineSettings frameNumber, 
                                                            PipelineSettings color,
                                                            PipelineSettings depth,
                                                            VkExtent2D resolution,
                                                            uint32_t frames)
  {
    std::vector<uint32_t> indices(0);

    assert(color == DEPTH && depth == ADDITIONAL_DEPTH && "Both pipeline settings set to depth! Likely not needed!\n");

    switch (pass)
    {
      case MAIN_COMP:
        indices.push_back(0);
        break;
      case MAIN_GEOM:
        indices.push_back(1);
        break;
      default:
        indices.push_back(createRenderPass());
    }

    uint32_t toRender;

    switch (frameNumber)
    {
      case SINGULAR:
        toRender = 1;
        synced.emplace(currentIndex, false);
        break;
      case SYNCED:
        toRender = TheSwapChain::MAX_FRAMES_IN_FLIGHT;
        synced.emplace(currentIndex, true);
        break;
      case NFRAMES:
        toRender = frames;
        break;
      default:
        assert("Incorrect frameNumber specification!\n");
    }

    switch (color)
    {
      case COLOR:
        for (int i = 0; i < toRender; i++)
        {
          uint32_t index = createColorImage();
          if (i == 0) indices.push_back(index);
          createFrameBuffer(index);
        }
        break;
      case DEPTH:
        for (int i = 0; i < toRender; i++)
        {
          uint32_t index = createDepthImage();
          if (i == 0) indices.push_back(index);
          createFrameBuffer(index);
        }
        break;
      default:
        assert("Incorrect color specification!\n");
    }

    if (depth == ADDITIONAL_DEPTH)
    {
      indices.push_back(createDepthImage());
    }


    currentIndex++;
    return indices;
  }

  uint32_t TheSwapChain::createDepthImage()
  {
    static uint32_t index = 0;

    return index++;
  }

  uint32_t TheSwapChain::createColorImage()
  {
    static uint32_t index = 0;

    return index++;
  }

  uint32_t TheSwapChain::createFrameBuffer(uint32_t idx)
  {
    static uint32_t index = 0;

    return index++;
  }

  uint32_t TheSwapChain::createRenderPass()
  {
    static uint32_t index = 0;

    return index++;
  }

  uint32_t TheSwapChain::createImageView()
  {
    static uint32_t index = 0;

    return index++;
  }

  void TheSwapChain::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(placeholderImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
            VK_SUCCESS ||
            vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
            VK_SUCCESS ||
            vkCreateFence(device.device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
  }

  VkSurfaceFormatKHR TheSwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && //
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
          return availableFormat;
      }
    }

        return availableFormats[0];
  }

    VkPresentModeKHR TheSwapChain::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes) {
        // for (const auto &availablePresentMode : availablePresentModes) {
        //   if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        //     std::cout << "Present mode: Mailbox" << std::endl;
        //     return availablePresentMode;
        //   }
        // }

        // for (const auto &availablePresentMode : availablePresentModes) {
        //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        //    std::cout << "Present mode: Immediate" << std::endl;
        //     return availablePresentMode;
        //   }
        // }

        std::cout << "Present mode: V-Sync" << std::endl;
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D TheSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            VkExtent2D actualExtent = windowExtent;
            actualExtent.width = std::max(
                capabilities.minImageExtent.width,
                std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(
                capabilities.minImageExtent.height,
                std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VkFormat TheSwapChain::findDepthFormat() {
        return device.findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

}
