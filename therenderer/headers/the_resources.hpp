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

      std::vector<VkDescriptorSet>& getSets() {return sets;}

      std::vector<std::unique_ptr<TheDescriptorPool>> pools;
                                                      //0 -> globalPool (usually just general use buffers. i.e: aspect ratio)
                                                      //1 -> texturePool
                                                      //2 -> imageBufferPool (final image, shadowmaps)
                                                      //3 -> imguiPool

      std::vector<std::unique_ptr<TheDescriptorSetLayout>> layouts;
                                                      //0 -> global buffer
                                                      //1 -> texture array
                                                      //2 -> image buffer array

      std::vector<VkDescriptorSet> sets;
                                                      //0 -> global buffer
                                                      //1 -> texture array
                                                      //2 -> image buffer array
    private:

      TheDevice& theDevice;

  };
}
