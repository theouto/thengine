#include "../headers/the_loop.hpp"

#include <iostream>
#include <vulkan/vulkan_core.h>

namespace the
{
  void TheLoop::render()
  {
    std::vector<uint32_t> resources = theRenderer.getNeededResources(TheSwapChain::COMP, TheSwapChain::SYNCED,
                                            TheSwapChain::COLOR, TheSwapChain::NO_ADDITIONAL_DEPTH, 
                                            VkExtent2D{defWidth, defHeight});

    ComputeSystem compute{theDevice, theRenderer.getFramePass(0),
                         "therenderer/shaders/compiled/present.comp.spv", VK_SHADER_STAGE_COMPUTE_BIT,
                         theRenderer.getSetLayout(2)->getDescriptorSetLayout(),
                         theRenderer.getSet(2), resources};

    std::cout << "rendering🙏: \n";

    while (theEvents.eventHandler())
    {
      keyboardController.processRegularKeys(theWindow.getSDLwindow());

      if (auto commandBuffer = theRenderer.beginFrame())
	  {
        FrameInfo frameInfo
        {
          0,
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
        frameInfo.imageIndex = theRenderer.getImageIndex();

        compute.render(frameInfo);

        theRenderer.beginSwapChainRenderPass(frameInfo.commandBuffer, 0);
        theRenderer.endSwapChainRenderPass(frameInfo.commandBuffer);

        theRenderer.endFrame();
      }
    }
  }
};
