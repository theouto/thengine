#include "../headers/the_render.hpp"

#include <iostream>
#include <cstdlib>
#include <vulkan/vulkan_core.h>

namespace the
{
  TheRender::TheRender(TheDevice& device, TheWindow& window) : theDevice{device}, theWindow{window}
  {
    theResources = std::make_unique<TheResources>(theDevice);
    recreateSwapChain();
    createCommandBuffers();
    initBaseImageBuffers();
  }

  TheRender::~TheRender(){freeCommandBuffers();}

  void TheRender::initBaseImageBuffers()
  {
    TheDescriptorWriter(*(theResources->layouts[2]), *(theResources->pools[2]))
      .build(theResources->sets[2]);
  }

  void TheRender::recreateResources()
  {
    for (int i = 0; i < pipelines.size(); i++)
    {
      auto c = pipelines[i];
      theSwapChain->createNeededResources(c.settings[0], c.settings[1], c.settings[2], c.settings[3],  theWindow.getExtent(), c.frames);
    }
  }

  void TheRender::recreateBuffers()
  {
    recreateResources();

    auto sacrifice = std::make_unique<TheDescriptorWriter>(*(theResources->layouts[2]), *(theResources->pools[2]));

    for (int i = theSwapChain->swapChainImageCount(); i < theSwapChain->getImageViewCount(); i++)
    {
      auto imageInfo = theResources->descriptorImageInfoHelper(theDevice, theSwapChain->getImageView(i));

      int idx = i - theSwapChain->swapChainImageCount();

      if (pipelines[idx].settings[0] == TheSwapChain::COMP) sacrifice->addImage(0, &imageInfo, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, idx);
         sacrifice->addImage(1, &imageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  idx)
         .overwrite(theResources->sets[2]);
    }
  }

  std::vector<uint32_t> TheRender::getNeededResources(TheSwapChain::PipelineSettings pass, 
                                               TheSwapChain::PipelineSettings frameNumber, 
                                               TheSwapChain::PipelineSettings color,
                                               TheSwapChain::PipelineSettings depth,
                                               VkExtent2D resolution,
                                               uint32_t frames)
  {
    auto returnee = theSwapChain->createNeededResources(pass, frameNumber, color, depth, theWindow.getExtent(), frames);

    if (frameNumber == TheSwapChain::SYNCED) frames = TheSwapChain::MAX_FRAMES_IN_FLIGHT;
    pipelines.push_back({{pass, frameNumber, color, depth}, theWindow.getExtent(), frames});

    auto sacrifice = std::make_unique<TheDescriptorWriter>(*(theResources->layouts[2]), *(theResources->pools[2]));

    if (pass != TheSwapChain::PRESENT)
    {
      for (int i = 0; i < frames; i++)
      {
        auto imageInfo = theResources->descriptorImageInfoHelper(theDevice, theSwapChain->getImageView(returnee[0] + i));

          if (pass == TheSwapChain::COMP) sacrifice->addImage(0, &imageInfo, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, returnee[0] + i);
          sacrifice->addImage(1, &imageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, returnee[0] + i)
          .overwrite(theResources->sets[2]);
      }
    }

    return returnee;
  }

  void TheRender::recreateSwapChain()
  {
    extent = theWindow.getExtent();
    while (extent.width == 0 || extent.height == 0) {
      extent = theWindow.getExtent();
      SDL_WaitEvent(nullptr);
    }
    vkDeviceWaitIdle(theDevice.device());

    if (theSwapChain == nullptr) {
      theSwapChain = std::make_unique<TheSwapChain>(theDevice, extent);
    } else {
      std::shared_ptr<TheSwapChain> oldSwapChain = std::move(theSwapChain);
      theSwapChain = std::make_unique<TheSwapChain>(theDevice, extent, oldSwapChain);
      recreateBuffers();

      if (!oldSwapChain->compareSwapFormats(*theSwapChain.get())) {
        throw std::runtime_error("Swap chain image(or depth) format has changed!");
      }
    }
  }

  void TheRender::createCommandBuffers() {
    commandBuffers.resize(TheSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = theDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(theDevice.device(), &allocInfo, commandBuffers.data()) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void TheRender::freeCommandBuffers() {
    vkFreeCommandBuffers(
        theDevice.device(),
        theDevice.getCommandPool(),
        static_cast<uint32_t>(commandBuffers.size()),
        commandBuffers.data());
    commandBuffers.clear();
  }

  VkCommandBuffer TheRender::beginFrame() {
    assert(!isFrameStarted && "Can't call beginFrame while already in progress");

    auto result = theSwapChain->acquireNextImage(&currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
  }

  void TheRender::endFrame() {
    assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }

    auto result = theSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || theWindow.wasWindowResized()) {
      theWindow.resetWindowResizedFlag();
      recreateSwapChain();
    }else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % TheSwapChain::MAX_FRAMES_IN_FLIGHT;
  }

  void TheRender::beginSwapChainRenderPass(VkCommandBuffer commandBuffer, uint32_t bufferIndex, uint32_t renderPassIndex) {
    assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = theSwapChain->getRenderPass(renderPassIndex);

    if (bufferIndex == 0)
    {
      renderPassInfo.framebuffer = theSwapChain->getFrameBuffer(bufferIndex + currentImageIndex);
    } else {
      renderPassInfo.framebuffer = theSwapChain->getFrameBuffer(bufferIndex +
                                   currentImageIndex % TheSwapChain::MAX_FRAMES_IN_FLIGHT * theSwapChain->isSynced(bufferIndex));
    }

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = theSwapChain->getImageExtent(bufferIndex);

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(theSwapChain->getImageExtent(bufferIndex).width);
    viewport.height = static_cast<float>(theSwapChain->getImageExtent(bufferIndex).height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, theSwapChain->getImageExtent(bufferIndex)};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }

  void TheRender::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't end render pass on command buffer from a different frame");
    vkCmdEndRenderPass(commandBuffer);
  }
};
