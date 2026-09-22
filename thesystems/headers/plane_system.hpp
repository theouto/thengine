#pragma once

#include "../../therenderer/headers/the_pipeline.hpp"
#include "../../therenderer/headers/the_frameinfo.hpp"
#include <vector>

namespace the
{
  class PlaneSystem
  {
    public:
      PlaneSystem(TheDevice& device, VkRenderPass renderPass,
                  std::vector<std::string> shaderPaths, std::vector<VkDescriptorSetLayout> globalSetLayout, 
                  std::vector<VkDescriptorSet> sets, std::vector<uint32_t> resources);
	  ~PlaneSystem();

	  PlaneSystem(const PlaneSystem&) = delete;
	  PlaneSystem& operator=(const PlaneSystem&) = delete;

      uint32_t getRenderPassIndex() {return resources[0];}
      uint32_t getBufferIndex() {return resources[1];}

	  void render(FrameInfo &frameInfo);
	private:
	  void createPipeLineLayout(std::vector<VkDescriptorSetLayout> &globalSetLayout);
	  void createPipeline(VkRenderPass renderPass, std::vector<std::string> shaderPaths);

	  TheDevice& theDevice;
      std::vector<VkDescriptorSet> sets;
	  std::unique_ptr<ThePipeline> thePipeline;
      std::vector<uint32_t> resources;
	  VkPipelineLayout pipelineLayout;
  };
}
