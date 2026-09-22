#include "../headers/opaque_geom.hpp"
//#include "shadow_system.hpp"
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <stdexcept>

namespace the
{
  struct SimplePushConstantData{};

  OpaqueGeometry::OpaqueGeometry(TheDevice& device, VkRenderPass renderPass, std::vector<std::string> paths,
                                 std::vector<VkDescriptorSetLayout> globalSetLayout, std::vector<VkDescriptorSet> sets,
                                 std::vector<uint32_t> resources) : theDevice{device} , sets{sets}, resources{resources}, filePaths{paths}
  {
  	createPipeLineLayout(globalSetLayout);
  	createPipeline(renderPass);
  }

  OpaqueGeometry::~OpaqueGeometry()
  {
  	vkDestroyPipelineLayout(theDevice.device(), pipelineLayout, nullptr);
  }

  void OpaqueGeometry::createPipeLineLayout(std::vector<VkDescriptorSetLayout> &globalSetLayout)
  {
    /*
  	VkPushConstantRange pushConstantRange{};
  	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  	pushConstantRange.offset = 0;
  	pushConstantRange.size = sizeof(SimplePushConstantData);
    */

  	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(globalSetLayout.size());
  	pipelineLayoutInfo.pSetLayouts = globalSetLayout.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
	//pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  	if (vkCreatePipelineLayout(theDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
  	{
      throw std::runtime_error("\n\nfailed to create pipeline layout\n\n");
  	}
  }

  void OpaqueGeometry::createPipeline(VkRenderPass renderPass)
  {
  	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  	PipelineConfigInfo pipelineConfig{};
  	ThePipeline::defaultPipelineConfigInfo(pipelineConfig);
	
  	pipelineConfig.renderPass = renderPass;
  	pipelineConfig.pipelineLayout = pipelineLayout;
    thePipeline = std::make_unique<ThePipeline>(theDevice, filePaths, pipelineConfig);
  }

  void OpaqueGeometry::renderGameObjects(FrameInfo &frameInfo)
  {
    thePipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
	    0, sets.size(), sets.data(), 0, nullptr);

    std::unordered_map<XXH32_hash_t, bool> render;

    for (auto& kv : frameInfo.gameObjects)
    {
	  auto& obj = kv.second;
	  if (obj.model == nullptr) continue;
      try {render.at(obj.instanceHash);} catch (std::out_of_range e)
      {
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);

        render.emplace(obj.instanceHash, true);
      }
    }
  }
}
