#include "../headers/the_render.hpp"
#include <vulkan/vulkan_core.h>

//the_render will encompass everything that has to do with logic and is found within VulkanEngine/the_renderer.cpp
//
//I will try to not make too much of a muck of it this time, keyword being try.
namespace the
{
  TheRender::TheRender(TheDevice& device, TheWindow& window) : theDevice{device}, theWindow{window}
  {
    theResources = std::make_unique<TheResources>(theDevice);
    recreateSwapChain();
    createCommandBuffers();
  }

  void TheRender::recreateBuffers()
  {
    int k = 0;

    for (int i = 0; i < theResources->presentedImages.size(); i++)
    {
      for (int j = 0; j < TheSwapChain::MAX_FRAMES_IN_FLIGHT; j++)
      {
        auto imageInfo = theResources->descriptorImageInfoHelper(theDevice, theSwapChain->getImageView(i+j));

        TheDescriptorWriter(*(theResources->layouts[2]), *(theResources->pools[2]))
          .addImage(0, &imageInfo, k++)
          .overwrite(theResources->presentedImages[i][j]);
      }
    }
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
      recreateSwapChain();

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

  void TheRender::beginSwapChainRenderPass(VkCommandBuffer commandBuffer, uint32_t bufferIndex) {
    assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = theSwapChain->getRenderPass(bufferIndex);
    renderPassInfo.framebuffer = theSwapChain->getFrameBuffer(bufferIndex + currentImageIndex * theSwapChain->isSynced(bufferIndex));

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
