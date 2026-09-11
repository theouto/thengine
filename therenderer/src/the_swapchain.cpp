#include "../headers/the_swapchain.hpp"

#include <cassert>
#include <vulkan/vulkan_core.h>

namespace the
{
  TheSwapChain::TheSwapChain(TheDevice &deviceRef, VkExtent2D windowExtent) : device{deviceRef}
  {
    createSwapChain();
  }

  TheSwapChain::TheSwapChain(TheDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<TheSwapChain> previous) : device{deviceRef}
  {
    createSwapChain();

    

    previous = nullptr;
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

}
