#pragma once

#include "../../therenderer/headers/the_pipeline.hpp"
#include "../../therenderer/headers/the_frameinfo.hpp"

namespace the
{
  class OpaqueSystem
  {
    public:
      OpaqueSystem(TheDevice& device, VkRenderPass renderPass,
                   std::vector<std::string> shaderPaths,
                   std::vector<VkDescriptorSetLayout> globalSetLayout);
	  ~OpaqueSystem();

	  OpaqueSystem(const OpaqueSystem&) = delete;
	  OpaqueSystem& operator=(const OpaqueSystem&) = delete;

	  void renderGameObjects(FrameInfo &frameInfo);
	private:
	  void createPipeLineLayout(std::vector<VkDescriptorSetLayout> &globalSetLayout);
	  void createPipeline(VkRenderPass renderPass);

      glm::mat4 lightSpaceMatrix{1.f};
	  TheDevice& theDevice;
	  std::unique_ptr<ThePipeline> thePipeline;
	  VkPipelineLayout pipelineLayout;
  };
}
