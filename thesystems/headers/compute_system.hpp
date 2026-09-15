#pragma once

#include "../../therenderer/headers/the_pipeline.hpp"
#include "../../therenderer/headers/the_frameinfo.hpp"
#include <vector>

namespace the
{
  class ComputeSystem
  {
    public:
      ComputeSystem(TheDevice& device, VkRenderPass renderPass,
                   std::vector<std::string> shaderPaths, VkShaderStageFlagBits stages,
                   std::vector<VkDescriptorSetLayout> globalSetLayout, std::vector<VkDescriptorSet>& sets);
	  ~ComputeSystem();

	  ComputeSystem(const ComputeSystem&) = delete;
	  ComputeSystem& operator=(const ComputeSystem&) = delete;

	  void render(FrameInfo &frameInfo);
	private:
	  void createPipeLineLayout(std::vector<VkDescriptorSetLayout> &globalSetLayout, VkShaderStageFlagBits stages);
	  void createPipeline(VkRenderPass renderPass, std::vector<std::string> shaderPaths);

	  TheDevice& theDevice;
      std::vector<VkDescriptorSet>& sets;
	  std::unique_ptr<ThePipeline> thePipeline;
	  VkPipelineLayout pipelineLayout;
  };
}
