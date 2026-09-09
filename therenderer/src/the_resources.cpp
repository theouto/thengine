#include "../headers/the_resources.hpp"

namespace the
{
  //This file will encompass all that is related to the resources found within VulkanEngine/lve_renderer.cpp
  //It makes more sense for them to be separate, rather than it all be a giant monolithic piece of code
  TheResources::TheResources(TheDevice& device) : theDevice{device}
  {
    imguiPool = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(20)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 400)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 400)
            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT)
            .build();

    descriptorPool = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(1)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1001) //need to make sure that I will never have to worry about this
            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .build();

    shadowPool = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(1)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4)
            .build();

    bindlessSetLayout = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1000, 
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            .addDescriptorFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)
            .build();

    shadowSetLayout = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 4, 
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            .addDescriptorFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)
            .build();

    colorBufferSetLayout = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 4, 
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            .addDescriptorFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)
            .build();

  }
};
