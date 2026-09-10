#pragma once

#include "../../theloading/headers/the_textures.hpp"

#include "the_descriptors.hpp"
#include "the_buffer.hpp"
#include "the_device.hpp"
#include "the_swapchain.hpp"

#include <memory>

namespace the
{
  class TheResources
  {
    public:
      TheResources(TheDevice& device);
      ~TheResources(){};
      static VkDescriptorImageInfo descriptorImageInfoHelper(TheDevice& device, VkImageView imageView);

      void generateDescriptors();
      void updateDescriptors();

      std::vector<std::vector<VkDescriptorSet>>& getPresentedImages() {return presentedImages;}

      std::vector<std::unique_ptr<TheDescriptorPool>> pools;
                                                      //0 -> globalPool (usually just general use buffers. i.e: aspect ratio)
                                                      //1 -> texturePool
                                                      //2 -> imageBufferPool (final image, shadowmaps)
                                                      //3 -> imguiPool

      std::vector<std::unique_ptr<TheDescriptorSetLayout>> layouts;
                                                      //0 -> global buffer
                                                      //1 -> texture array
                                                      //2 -> image buffer array

      //anything that is rendered into an imageView/storage
      std::vector<std::vector<VkDescriptorSet>> presentedImages;
    private:

      TheDevice& theDevice;

  };
}
