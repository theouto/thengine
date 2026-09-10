#pragma once

#include "../../theloading/headers/the_textures.hpp"

#include "the_descriptors.hpp"
#include "the_buffer.hpp"
#include "the_device.hpp"
#include <memory>

namespace the
{
  class TheResources
  {
    public:
      TheResources(TheDevice& device);
      ~TheResources(){};
      static VkDescriptorImageInfo descriptorImageInfoHelper(TheDevice& device, VkImageView imageView);

    private:
      void generateDescriptors();
      void updateDescriptors();

      TheDevice& theDevice;

      VkDescriptorSet bindlessSet;
      VkDescriptorSet shadowSet;

      std::vector<std::unique_ptr<TheDescriptorPool>> pools;
                                                      //0 -> globalPool
                                                      //1 -> texturePool
                                                      //2 -> imageBufferPool
                                                      //3 -> imguiPool

      std::vector<std::unique_ptr<TheDescriptorSetLayout>> layouts;
                                                      //0 -> globalPool
                                                      //1 -> texturePool
                                                      //2 -> imageBufferPool

      std::vector<std::vector<VkDescriptorSet>> presentedImages;
                                                      //0 -> finalImage
                                                      //1 ->
  };
}
