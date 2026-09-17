#include "../headers/plane_system.hpp"
#include <vulkan/vulkan_core.h>

namespace the
{
  PlaneSystem::PlaneSystem(TheDevice& device, VkRenderPass renderPass,
                           std::vector<std::string> shaderPaths,
                           std::vector<VkDescriptorSetLayout> globalSetLayout, std::vector<VkDescriptorSet> sets) : theDevice{device} , sets{sets}
  {
    createPipeLineLayout(globalSetLayout);
	createPipeline(renderPass, shaderPaths);
  }

  PlaneSystem::~PlaneSystem() {vkDestroyPipelineLayout(theDevice.device(), pipelineLayout, nullptr);}

  void PlaneSystem::createPipeLineLayout(std::vector<VkDescriptorSetLayout>& globalSetLayout)
  {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(globalSetLayout.size());
	pipelineLayoutInfo.pSetLayouts = globalSetLayout.data();

	if (vkCreatePipelineLayout(theDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
	  throw std::runtime_error("\n\nfailed to create pipeline layout\n\n");
	}
  }

  void PlaneSystem::createPipeline(VkRenderPass renderPass, std::vector<std::string> shaderPaths)
  {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
	ThePipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	thePipeline = std::make_unique<ThePipeline>(theDevice, shaderPaths, pipelineConfig);

  }

  void PlaneSystem::render(FrameInfo& frameInfo)
  {
    thePipeline->bind(frameInfo.commandBuffer);
    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0, sets.size(), sets.data(), 0, nullptr);
    vkCmdDraw(frameInfo.commandBuffer, 3, 1, 0, 0);
  }
}
