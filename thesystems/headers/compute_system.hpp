#pragma once

#include "../../therenderer/headers/the_pipeline.hpp"
#include "../../therenderer/headers/the_frameinfo.hpp"
#include <cstdint>
#include <vector>

namespace the
{
  class ComputeSystem
  {
    public:
      ComputeSystem(TheDevice& device, VkRenderPass renderPass,
                   std::string shaderPath,
                   std::vector<VkDescriptorSetLayout> globalSetLayout, std::vector<VkDescriptorSet> sets,
                   std::vector<uint32_t> resources);
	  ~ComputeSystem();

	  ComputeSystem(const ComputeSystem&) = delete;
	  ComputeSystem& operator=(const ComputeSystem&) = delete;

	  void render(FrameInfo &frameInfo);
      uint32_t getBufferIndex() {return resources[0];}
	private:
	  void createPipeLineLayout(std::vector<VkDescriptorSetLayout> &globalSetLayout);
	  void createPipeline(VkRenderPass renderPass, std::string shaderPaths);

	  TheDevice& theDevice;

      std::vector<uint32_t> resources;
      std::vector<VkDescriptorSet> sets;

	  std::unique_ptr<ThePipeline> thePipeline;
	  VkPipelineLayout pipelineLayout;
  };
}
