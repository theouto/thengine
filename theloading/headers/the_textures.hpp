#pragma once

#include <utility>

#include "../../therenderer/headers/the_device.hpp"
#include "../../therenderer/headers/the_descriptors.hpp"
#include "../../therenderer/headers/the_buffer.hpp"
#include "../../thelogic/headers/the_window.hpp"

namespace the
{
	class TheTextures
	{
	public:

        enum texType
        {
          COLOR,
          NORMAL,
          SINGLE_UNORM
        };
 
		TheTextures(TheDevice& device, const std::string path, texType tType);
        ~TheTextures();

		void createTextureImage();
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		VkImageView createImageView(VkImage image, VkFormat format);
		void createTextureImageView();
		void createTextureSampler();
		void generateMipmaps(int32_t texWidth, int32_t texHeight);
        texType getTexType() {return tType;}

        static void createTextureSampler(TheDevice& device, VkSampler& sampler);

		//getter functions, used for destruction
		VkImage& getTextureImage() { return textureImage; }
		VkDeviceMemory& getTextureImageMemory() { return textureImageMemory; }
		VkImageView& getTextureImageView() { return textureImageView; }
		VkSampler& getSampler() { return textureSampler; }
		VkDescriptorImageInfo getDescriptorInfo() 
		{

			VkDescriptorImageInfo descriptorInfo{};

			descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptorInfo.imageView = textureImageView;
			descriptorInfo.sampler = textureSampler;

			return descriptorInfo; 
		}

	private:

		uint32_t mipLevels;
		VkFormat textureFormat;
		texType tType;
        std::string filePath;
        std::pair<int, int> resolution;

		TheDevice& theDevice;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageCreateInfo imageInfo{};
		VkImageView textureImageView;
		VkSampler textureSampler;

    };
}
