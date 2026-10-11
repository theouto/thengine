#pragma once

#include "../../therenderer/headers/the_device.hpp"
#include "../../therenderer/headers/the_buffer.hpp"
#include "../../theloading/headers/the_textures.hpp"

#include "../../thirdparty/xxHash/xxhash.h"
#include "../../thirdparty/tinygltf/tiny_gltf_v3.h"

#include <glm/ext/vector_float3.hpp>

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
			glm::vec3 normal{};
			glm::vec3 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator ==(const Vertex& other) const { return position == other.position //&& color == other.color 
				&& normal == other.normal && uv == other.uv; }
		};

        struct InstanceData
        {
          alignas(16) glm::vec3 translation{0.f};
          alignas(16) glm::vec3 rotation{0.f};
          alignas(16) glm::vec3 scale{1.f};

          alignas(16) glm::ivec3 RIDone{1};
          alignas(16) glm::ivec3 RIDtwo{1, 1, 0};
          glm::vec4 modifiers{1.f, 1.f, 1.f, 0.f};

          static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
          static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

		struct Builder
		{
		  std::vector<Vertex> vertices{};
		  std::vector<uint32_t> indices{};

          std::shared_ptr<tinygltf3::Model> glTF = nullptr;

		  void loadModel(const std::string& filepath);
          void loadObj(const std::string& filepath);
          void loadGltf(const std::string& filepath);
		};

		TheModel(TheDevice & device, const TheModel::Builder &builder);
		~TheModel();

		TheModel(const TheModel&) = delete;
		TheModel& operator=(const TheModel&) = delete;

      	VkDescriptorPool descriptorPool;

		std::vector<VkDescriptorSet> descriptorSets;

		static std::unique_ptr<TheModel> createModelFromFile(TheDevice& device, const std::string &filepath);

        std::vector<tg3_image> getTextures();
        uint32_t addInstanceData(glm::vec3 scale, glm::vec3 translation, glm::vec3 rotation, std::vector<uint32_t> material, std::vector<float> materialModifiers);

        void createInstanceBuffer();
        void updateBuffer();

        void setScale(uint32_t index, glm::vec3 scale) {instanceData[index].scale = scale;}
        void setTranslation(uint32_t index, glm::vec3 translation) {instanceData[index].translation = translation;}
        void setRotation(uint32_t index, glm::vec3 rotation) {instanceData[index].rotation = rotation;}

        void removeglTF() {glTF = nullptr;}

        void setMaterial(uint32_t index, uint32_t* RID, float* modi)
        {
          instanceData[index].RIDone = glm::vec3{RID[0], RID[1], RID[2]};
          instanceData[index].RIDtwo = glm::vec3{RID[3], RID[4], RID[5]};
          instanceData[index].modifiers = glm::vec4{modi[0], modi[1], modi[2], modi[3]};
        }

        void flush() {instanceBuffer->flush();}

        uint32_t getInstanceCount() {return instanceData.size();}

		void bind(VkCommandBuffer);
		void draw(VkCommandBuffer);

        void updateInstances();
        void updateMaterial();

	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
        void createFromGltf();
		
        std::vector<std::unique_ptr<TheTextures>> textures;
        TheDevice &theDevice;

        bool createdbuffer = false;
		std::unique_ptr<TheBuffer> vertexBuffer;
		uint32_t vertexCount;

        XXH32_hash_t model_name;
        XXH32_hash_t material_name;

        //std::vector<tinyobj::mesh_t> meshes;
        int meshes = 0;
        std::vector<InstanceData> instanceData;
        std::vector<tg3_image> gltfImages;
        std::unique_ptr<TheBuffer> instanceBuffer = nullptr;

        std::shared_ptr<tinygltf3::Model> glTF = nullptr;

		bool hasIndexBuffer = false;
		std::unique_ptr<TheBuffer> indexBuffer;
		uint32_t indexCount;
	};
}
