#include "../headers/compute_system.hpp"
#include <vulkan/vulkan_core.h>

namespace the
{
  struct ComputeData
  {
    int width;
    int height;
  };

  ComputeSystem::ComputeSystem(TheDevice& device, VkRenderPass renderPass,
                             std::string shaderPath, VkShaderStageFlagBits stages,
                             VkDescriptorSetLayout globalSetLayout, VkDescriptorSet sets,
                             std::vector<uint32_t> resources) : theDevice{device}
  {
    this->sets = std::vector<VkDescriptorSet>{sets};
    std::vector<VkDescriptorSetLayout> sacrifice = {globalSetLayout};
    createPipeLineLayout(sacrifice, stages);
	createPipeline(renderPass, shaderPath);
  }

  ComputeSystem::~ComputeSystem() {vkDestroyPipelineLayout(theDevice.device(), pipelineLayout, nullptr);}

  void ComputeSystem::createPipeLineLayout(std::vector<VkDescriptorSetLayout>& globalSetLayout, VkShaderStageFlagBits stages)
  {
    VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = stages;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(ComputeData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(globalSetLayout.size());
	pipelineLayoutInfo.pSetLayouts = globalSetLayout.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(theDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
	  throw std::runtime_error("\n\nfailed to create pipeline layout\n\n");
	}
  }

  void ComputeSystem::createPipeline(VkRenderPass renderPass, std::string shaderPaths)
  {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
	ThePipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	thePipeline = std::make_unique<ThePipeline>(theDevice, shaderPaths, pipelineConfig);

  }

  void ComputeSystem::render(FrameInfo& frameInfo)
  {
    thePipeline->bindCompute(frameInfo.commandBuffer);
    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout,
	  0, sets.size(), sets.data(), 0, nullptr);

    ComputeData data{frameInfo.width, frameInfo.height};

    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputeData), &data);

    //vkCmdBeginPerTileExecutionQCOM(frameInfo.commandBuffer, nullptr);

    vkCmdDispatch(frameInfo.commandBuffer, std::ceil(frameInfo.width / 24.0),
                  std::ceil(frameInfo.height / 24.0), 1);

    //vkCmdEndPerTileExecutionQCOM(frameInfo.commandBuffer, nullptr);
  }
}
