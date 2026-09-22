#include "../headers/the_loop.hpp"

#include <iostream>
#include <chrono>
#include <vulkan/vulkan_core.h>

namespace the
{
  void TheLoop::render()
  {
    uboBuffers.resize(TheSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++)
    {
      uboBuffers[i] = std::make_unique<TheBuffer>(
        theDevice,
        sizeof(GlobalUbo),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

      uboBuffers[i]->map();
    }

    theRenderer.loadUboInfo(uboBuffers);

    //For anyone seeing this: I will not be repeating the errors of the past, this is a placeholder until I know that things work as they should
    //Actually no, these are not the mistakes of the past, or at least I don't think they are
    std::vector<uint32_t> resources = theRenderer.getNeededResources(TheSwapChain::COMP, TheSwapChain::SINGULAR,
                                            TheSwapChain::COLOR, TheSwapChain::NO_ADDITIONAL_DEPTH,
                                            VkExtent2D{defWidth, defHeight});

    ComputeSystem compute{theDevice, theRenderer.getFramePass(resources[0]),
                         defShaderPath + "present.comp.spv", 
                         {theRenderer.getSetLayout(1)->getDescriptorSetLayout(),
                         theRenderer.getSetLayout(2)->getDescriptorSetLayout()}, resources};

    resources = theRenderer.getNeededResources(TheSwapChain::PRESENT, TheSwapChain::SYNCED,
                                            TheSwapChain::COLOR, TheSwapChain::NO_ADDITIONAL_DEPTH,
                                            VkExtent2D{defWidth, defHeight});

    PlaneSystem present{theDevice, theRenderer.getFramePass(resources[0]),
                        {defShaderPath + "final_present.vert.spv", defShaderPath + "final_present.frag.spv"},
                        {theRenderer.getSetLayout(1)->getDescriptorSetLayout(),
                        theRenderer.getSetLayout(2)->getDescriptorSetLayout()}, resources};

    
    resources = theRenderer.getNeededResources(TheSwapChain::GEOM, TheSwapChain::SYNCED,
                                               TheSwapChain::COLOR, TheSwapChain::ADDITIONAL_DEPTH,
                                               VkExtent2D{defWidth, defHeight});

    OpaqueGeometry render{theDevice, theRenderer.getFramePass(resources[0]),
                          {defShaderPath + "main_geom.vert.spv", defShaderPath + "main_geom.frag.spv"},
                          {theRenderer.getSetLayout(0)->getDescriptorSetLayout(),
                          theRenderer.getSetLayout(1)->getDescriptorSetLayout(),
                          theRenderer.getSetLayout(2)->getDescriptorSetLayout()},
                          resources};
    

    auto viewerObject = TheGameObject::createGameObject();
    viewerObject.transform.translation.z = -1.5f;

    sceneManager.load("scenes/light_test.ths", *(theRenderer.theResources->pools[1]));
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::cout << "rendering🙏: \n\n\n";

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
      float aspect = static_cast<float>(theWindow.getExtent().width)/theWindow.getExtent().height;
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, defNear, defFar);

      if (auto commandBuffer = theRenderer.beginFrame())
	  {
        FrameInfo frameInfo
        {
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
        frameInfo.width = theWindow.getExtent().width;
        frameInfo.height = theWindow.getExtent().height;

        std::vector<VkDescriptorSet> sets = {theRenderer.theResources->sets[frameInfo.frameIndex],
                                             theRenderer.theResources->sets[2],
                                             theRenderer.theResources->sets[3]};
        frameInfo.sets = sets;

        GlobalUbo ubo{};
        ubo.projection = camera.getProjection();
        ubo.view = camera.getView();
        ubo.viewStat = camera.getviewStat();
        ubo.inverseView = camera.getInverseView();
        ubo.width = theWindow.getExtent().width;
        ubo.height = theWindow.getExtent().height;

        uboBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameInfo.frameIndex]->flush();

        theRenderer.beginSwapChainRenderPass(frameInfo.commandBuffer, render.getBufferIndex(), render.getRenderPassIndex());
        render.renderGameObjects(frameInfo);
        theRenderer.endSwapChainRenderPass(frameInfo.commandBuffer);

        compute.render(frameInfo);

        theRenderer.beginSwapChainRenderPass(frameInfo.commandBuffer, present.getBufferIndex(), present.getRenderPassIndex());
        present.render(frameInfo);
        theRenderer.endSwapChainRenderPass(frameInfo.commandBuffer);

        theRenderer.endFrame();
      }
    }
    vkDeviceWaitIdle(theDevice.device());
  }
};
