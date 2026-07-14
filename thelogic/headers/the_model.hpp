#pragma once

#include "../../therenderer/headers/the_device.hpp"
#include "../../therenderer/headers/the_buffer.hpp"
#include "../../theloading/headers/the_textures.hpp"
#include "../headers/the_model.hpp"
#include "the_utils.hpp"



#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <vector>

namespace the
{
	class TheModel
	{

	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator ==(const Vertex& other) const { return position == other.position && color == other.color 
				&& normal == other.normal && uv == other.uv; }
		};

		struct UniformBufferObject {
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 proj;
		}; 

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

        
		TheModel(TheDevice & device, const TheModel::Builder &builder);
		~TheModel();

		TheModel(const TheModel&) = delete;
		TheModel& operator=(const TheModel&) = delete;
	    
      	VkDescriptorPool descriptorPool;

		std::vector<VkDescriptorSet> descriptorSets;

		static std::unique_ptr<TheModel> createModelFromFile(TheDevice& device, const std::string &filepath);

		void bind(VkCommandBuffer);
		void draw(VkCommandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
		
        std::vector<std::unique_ptr<TheTextures>> textures;
        TheDevice &theDevice;

		std::unique_ptr<TheBuffer> vertexBuffer;
		uint32_t vertexCount;
		
		bool hasIndexBuffer = false;
		std::unique_ptr<TheBuffer> indexBuffer;
		uint32_t indexCount;
	};
}
