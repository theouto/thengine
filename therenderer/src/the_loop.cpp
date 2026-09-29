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
    resourced.push_back(theRenderer.getNeededResources(TheSwapChain::COMP, TheSwapChain::SYNCED,
                                            TheSwapChain::COLOR, TheSwapChain::NO_ADDITIONAL_DEPTH,
                                            theWindow.getExtent()));

    ComputeSystem compute{theDevice, theRenderer.getFramePass(resourced[resourced.size() - 1][0]),
                         defShaderPath + "present.comp.spv", 
                         {theRenderer.getSetLayout(0)->getDescriptorSetLayout(),
                        theRenderer.getSetLayout(1)->getDescriptorSetLayout(),
                        theRenderer.getSetLayout(2)->getDescriptorSetLayout()}, resourced[resourced.size() - 1]};

    resourced.push_back(theRenderer.getNeededResources(TheSwapChain::PRESENT, TheSwapChain::SYNCED,
                                            TheSwapChain::COLOR, TheSwapChain::NO_ADDITIONAL_DEPTH,
                                            theWindow.getExtent()));

    PlaneSystem present{theDevice, theRenderer.getFramePass(resourced[resourced.size() - 1][0]),
                        {defShaderPath + "final_present.vert.spv", defShaderPath + "final_present.frag.spv"},
                        {theRenderer.getSetLayout(0)->getDescriptorSetLayout(),
                        theRenderer.getSetLayout(1)->getDescriptorSetLayout(),
                        theRenderer.getSetLayout(2)->getDescriptorSetLayout()}, resourced[resourced.size() - 1]};


    resourced.push_back(theRenderer.getNeededResources(TheSwapChain::GEOM, TheSwapChain::SYNCED,
                                               TheSwapChain::COLOR, TheSwapChain::ADDITIONAL_DEPTH,
                                               theWindow.getExtent()));

    OpaqueGeometry render{theDevice, theRenderer.getFramePass(resourced[resourced.size() - 1][0]),
                          {defShaderPath + "main_geom.vert.spv", defShaderPath + "main_geom.frag.spv"},
                          {theRenderer.getSetLayout(0)->getDescriptorSetLayout(),
                          theRenderer.getSetLayout(1)->getDescriptorSetLayout(),
                          theRenderer.getSetLayout(2)->getDescriptorSetLayout()},
                          resourced[resourced.size() - 1]};

    resourced.push_back(theRenderer.getNeededResources(TheSwapChain::GEOM, TheSwapChain::SYNCED,
                                               TheSwapChain::DEPTH, TheSwapChain::NO_ADDITIONAL_DEPTH,
                                               theWindow.getExtent()));

    OpaqueGeometry depth{theDevice, theRenderer.getFramePass(resourced[resourced.size() - 1][0]),
                          {defShaderPath + "depth.vert.spv", defShaderPath + "depth.frag.spv"},
                          {theRenderer.getSetLayout(0)->getDescriptorSetLayout(),
                          theRenderer.getSetLayout(1)->getDescriptorSetLayout(),
                          theRenderer.getSetLayout(2)->getDescriptorSetLayout()},
                          resourced[resourced.size() - 1]};

    VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

    auto viewerObject = TheGameObject::createGameObject();
    viewerObject.transform.translation.z = -1.5f;

    sceneManager.load("scenes/test_scene.ths", *(theRenderer.theResources->pools[1]));
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::cout << "rendering🙏: \n\n\n";

    while (theEvents.eventHandler())
    {
      auto newTime = std::chrono::high_resolution_clock::now();
      float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
      currentTime = newTime;

      keyboardController.processRegularKeys(theWindow.getSDLwindow());
      keyboardController.moveInPlaneXZ(frameTime, theWindow.getSDLwindow(), viewerObject,
                                       theWindow.getExtent().width/2.0, theWindow.getExtent().height/2.0);

      if (keyboardController.mousecontrol) SDL_WarpMouseInWindow(theWindow.getSDLwindow(),
                                            (theWindow.getExtent().width/2.0),
                                            (theWindow.getExtent().height/2.0));

      camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
      float aspect = static_cast<float>(theWindow.getExtent().width)/theWindow.getExtent().height;
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, defNear, defFar);

      if (auto commandBuffer = theRenderer.beginFrame())
	  {
        barrier.image = theRenderer.getImage(theRenderer.getFrameIndex() + theRenderer.getSwapChainImageCount());

        FrameInfo frameInfo
        {
          0,
          frameTime,
          0,
          0,
          commandBuffer,
          camera,
          gameObjects,
          sceneManager.handler(),
          barrier,
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
        ubo.frameIndex = frameInfo.frameIndex;
        ubo.near = 0.1f;
        ubo.far = 500.f;

        ubo.pointLights[0] = PointLight{glm::vec4(1.5f, -1.2f, 0.f, 1.f), glm::vec4(2.f, 0.f, 0.f, 1.f)};
        ubo.numLights = 1;

        uboBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameInfo.frameIndex]->flush();

        theRenderer.beginSwapChainRenderPass(frameInfo.commandBuffer, depth.getBufferIndex(), depth.getRenderPassIndex());
        depth.renderGameObjects(frameInfo);
        theRenderer.endSwapChainRenderPass(frameInfo.commandBuffer);

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
