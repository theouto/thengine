#include "../headers/the_loop.hpp"

#include <iostream>
#include <vulkan/vulkan_core.h>

namespace the
{
  void TheLoop::render()
  {
    auto resources = theRenderer.getNeededResources(TheSwapChain::MAIN_COMP, TheSwapChain::SYNCED, 
                                            TheSwapChain::COLOR, TheSwapChain::NO_ADDITIONAL_DEPTH, 
                                            VkExtent2D{1920, 1080});

    ComputeSystem compute{theDevice, theRenderer.getFramePass(resources[0]),
                         "therenderer/shaders/compiled/present.comp.spv", VK_SHADER_STAGE_COMPUTE_BIT,
                         theRenderer.getSetLayout(2)->getDescriptorSetLayout(),
                         theRenderer.getSet(2), resources};

    while (theEvents.eventHandler())
    {
      if (auto commandBuffer = theRenderer.beginFrame())
	  {
        FrameInfo frameInfo
        {
          0,
          0,
          0,
          0,
          commandBuffer,
          camera,
          nullptr,
          nullptr,
          nullptr,
          gameObjects,
          sceneManager.handler()
        };
        frameInfo.frameIndex = theRenderer.getFrameIndex();

        theRenderer.beginSwapChainRenderPass(commandBuffer, compute.getBufferIndex());
        compute.render(frameInfo);
        theRenderer.endSwapChainRenderPass(commandBuffer);
        theRenderer.endFrame();
      }
    }
  }
};
