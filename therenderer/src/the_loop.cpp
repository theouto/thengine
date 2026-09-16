#include "../headers/the_loop.hpp"

#include <iostream>
#include <chrono>
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

    auto viewerObject = TheGameObject::createGameObject();
    viewerObject.transform.translation.z = -1.5f;

    sceneManager.load("scenes/light_test.ths", *(theRenderer.theResources->pools[2]));
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::cout << "rendering🙏: \n";

    while (theEvents.eventHandler())
    {
      auto newTime = std::chrono::high_resolution_clock::now();
      float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
      currentTime = newTime;

      keyboardController.processRegularKeys(theWindow.getSDLwindow());
      keyboardController.moveInPlaneXZ(frameTime, theWindow.getSDLwindow(), viewerObject,
                                       theWindow.getExtent().width, theWindow.getExtent().height);

      if (keyboardController.mousecontrol) SDL_WarpMouseInWindow(theWindow.getSDLwindow(),
                                            (theWindow.getExtent().width/2.0),
                                            (theWindow.getExtent().height/2.0));

      camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

      if (auto commandBuffer = theRenderer.beginFrame())
	  {
        FrameInfo frameInfo
        {
          0,
          0,
          frameTime,
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
        frameInfo.width = theWindow.getExtent().width;
        frameInfo.height = theWindow.getExtent().height;

        compute.render(frameInfo);

        theRenderer.beginSwapChainRenderPass(frameInfo.commandBuffer, 0);
        theRenderer.endSwapChainRenderPass(frameInfo.commandBuffer);

        theRenderer.endFrame();
      }
    }
  }
};
