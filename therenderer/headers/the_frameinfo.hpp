#pragma once

#include "the_camera.hpp"
#include "../../thelogic/headers/the_game_object.hpp"
#include "../../theloading/headers/the_scene.hpp"

#include <vulkan/vulkan.h>

namespace the
{
	#define MAX_LIGHTS 10

	struct PointLight
	{
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct GlobalUbo
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
        glm::mat4 viewStat{ 1.f };
        glm::mat4 lightSpaceMatrix[4]{1.f, 1.f, 1.f, 1.f};
        glm::vec3 lightPos{-1.f, 2.f, -1.f};
        int numLights;
        glm::vec4 depthValues;
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f }; //RGB Intensity
		PointLight pointLights[MAX_LIGHTS]; 
	};

	struct FrameInfo
	{
		int frameIndex;
        int imageIndex;
		float frameTime;
        int width;
        int height;
		VkCommandBuffer commandBuffer;
		TheCamera& camera;
		VkDescriptorSet globalDescriptorSet;
        VkDescriptorSet bindlessSet;
        VkDescriptorSet shadowSet;
		TheGameObject::Map &gameObjects;
        TheMaterials materials;
	};
}
