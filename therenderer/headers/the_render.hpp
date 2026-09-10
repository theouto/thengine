#pragma once

#include "../../theloading/headers/the_textures.hpp"

#include "the_resources.hpp"
#include "the_camera.hpp"
#include "the_device.hpp"
#include "the_swapchain.hpp"

namespace the
{
  class TheRender
  {
    public:
      TheRender(TheDevice& device, TheWindow& window);
      ~TheRender();

      TheRender(const TheRender&) = delete;
      TheRender& operator=(const TheRender&) = delete;

      VkCommandBuffer getCurrentCommandBuffer() const
	  {
		assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
		return commandBuffers[currentFrameIndex];
	  }

      int getFrameIndex() const
	  {
		assert(isFrameStarted && "Cannot get frame index when frame not in progress");
		return currentFrameIndex;
	  }

      VkCommandBuffer beginFrame();
	  void endFrame();
	  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer, uint32_t bufferIndex);
	  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

      VkRenderPass getFramePass(uint32_t index) {return theSwapChain->getRenderPass(index);}
      VkDescriptorImageInfo getImageInfo(uint32_t index) {return TheResources::descriptorImageInfoHelper(theDevice, theSwapChain->getImageView(index));}

    private:

      void generateDescriptors();
	  void createCommandBuffers();
	  void freeCommandBuffers();
	  void recreateSwapChain();
      void recreateBuffers();

	  TheWindow& theWindow;
      TheDevice& theDevice;
      std::vector<VkDescriptorBufferInfo> uboInfo;
      std::vector<std::shared_ptr<TheTextures>> textures;

      std::unique_ptr<TheResources> theResources;
      std::unique_ptr<TheSwapChain> theSwapChain;
	  std::vector<VkCommandBuffer> commandBuffers;

      VkExtent2D extent = {0, 0};
	  uint32_t currentImageIndex;
	  int currentFrameIndex{0};
	  bool isFrameStarted{false};
  };
}
