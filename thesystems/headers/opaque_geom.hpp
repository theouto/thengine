#pragma once

#include "../../therenderer/headers/the_pipeline.hpp"
#include "../../therenderer/headers/the_frameinfo.hpp"
//#include "./shadow_system.hpp"

#include <memory>
#include <vector>

namespace the
{
  class OpaqueGeometry
  {
	public:
      OpaqueGeometry(TheDevice& device, VkRenderPass renderPass, std::vector<std::string> paths,
                       std::vector<VkDescriptorSetLayout> globalSetLayout);
	  ~OpaqueGeometry();

	  OpaqueGeometry(const OpaqueGeometry&) = delete;
	  OpaqueGeometry& operator=(const OpaqueGeometry&) = delete;

	  void renderGameObjects(FrameInfo &frameInfo);
	private:
	  void createPipeLineLayout(std::vector<VkDescriptorSetLayout> &globalSetLayout);
	  void createPipeline(VkRenderPass renderPass);

      glm::mat4 lightSpaceMatrix{1.f};
      std::vector<std::string> filePaths;
	  TheDevice& theDevice;
	  std::unique_ptr<ThePipeline> thePipeline;
	  VkPipelineLayout pipelineLayout;
  };
}
