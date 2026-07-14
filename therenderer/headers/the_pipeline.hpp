#pragma once

#include <string>
#include <vector>

#include "the_device.hpp"

namespace the
{
	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkComputePipelineCreateInfo computeInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};


	class ThePipeline
	{
	public:
		ThePipeline(TheDevice &device, std::vector<std::string> filePaths, const PipelineConfigInfo& configInfo);
        ThePipeline(TheDevice &device, std::string filePath, const PipelineConfigInfo& configInfo);
		~ThePipeline();

		ThePipeline(const ThePipeline&) = delete;
		ThePipeline& operator=(const ThePipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
        void bindCompute(VkCommandBuffer commandBuffer);
		
		static void enableMSAA(PipelineConfigInfo& configInfo);
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
        static void defaultPipelineShadowInfo(PipelineConfigInfo &configInfo);
		static void enableAlphaBlending(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo);

        void createComputePipeline(const std::string& filePath, const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		TheDevice& theDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
        VkShaderModule computeModule;
	};
}
