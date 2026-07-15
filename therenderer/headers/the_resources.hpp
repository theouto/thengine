#pragma once

#include "../../theloading/headers/the_textures.hpp"

#include "the_descriptors.hpp"
#include "the_buffer.hpp"
#include "the_device.hpp"

namespace the
{
  class TheResources
  {
    public:
      TheResources(TheDevice& device);
      ~TheResources(){};
      static VkDescriptorImageInfo descriptorImageInfoHelper(TheDevice& device, VkImageView imageView);

    private:
      TheDevice& theDevice;

      VkDescriptorSet bindlessSet;
      VkDescriptorSet shadowSet;

      std::unique_ptr<TheDescriptorPool> descriptorPool = nullptr;
      std::unique_ptr<TheDescriptorPool> shadowPool = nullptr;
      std::unique_ptr<TheDescriptorSetLayout> shadowSetLayout = nullptr;
      std::unique_ptr<TheDescriptorSetLayout> colorBufferSetLayout = nullptr;
      std::unique_ptr<TheDescriptorSetLayout> bindlessSetLayout = nullptr;
  };
}
