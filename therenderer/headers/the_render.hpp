#pragma once

#include "../../theloading/headers/the_textures.hpp"

#include "the_resources.hpp"
#include "the_camera.hpp"
#include "the_device.hpp"
#include "the_swapchain.hpp"
#include <vulkan/vulkan_core.h>

namespace the
{
  class TheRender
  {
    public:

      struct PipelineMap
      {
        std::vector<TheSwapChain::PipelineSettings> settings;
        VkExtent2D resolution;
        uint32_t frames;
      };

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

      int getImageIndex() const
      {
        assert(isFrameStarted && "Cannot get frame index when frame not in progress");
        return currentImageIndex;
      }

      VkCommandBuffer beginFrame();
	  void endFrame();
	  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer, uint32_t bufferIndex);
	  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

      std::vector<uint32_t> getNeededResources(TheSwapChain::PipelineSettings pass, 
                                               TheSwapChain::PipelineSettings frameNumber, 
                                               TheSwapChain::PipelineSettings color,
                                               TheSwapChain::PipelineSettings depth,
                                               VkExtent2D resolution,
                                               uint32_t frames = 0);

      VkRenderPass getFramePass(uint32_t index) {return theSwapChain->getRenderPass(index);}
      VkDescriptorImageInfo getImageInfo(uint32_t index) {return TheResources::descriptorImageInfoHelper(theDevice, theSwapChain->getImageView(index));}

      std::shared_ptr<TheDescriptorSetLayout> getSetLayout(uint32_t index){return theResources->layouts[index];}
      VkDescriptorSet& getSet(uint32_t index){return theResources->sets[index];}

      std::unique_ptr<TheResources> theResources;

    private:

	  void createCommandBuffers();
      void initBaseImageBuffers();
	  void freeCommandBuffers();
	  void recreateSwapChain();
      void recreateBuffers();
      void recreateResources();

	  TheWindow& theWindow;
      TheDevice& theDevice;
      std::vector<VkDescriptorBufferInfo> uboInfo;
      std::vector<std::shared_ptr<TheTextures>> textures;

      std::vector<PipelineMap> pipelines;

      std::unique_ptr<TheSwapChain> theSwapChain;
	  std::vector<VkCommandBuffer> commandBuffers;

      VkExtent2D extent = {0, 0};
	  uint32_t currentImageIndex;
	  int currentFrameIndex{0};
	  bool isFrameStarted{false};
  };
}
