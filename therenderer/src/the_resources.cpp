#include "../headers/the_resources.hpp"
#include <cinttypes>
#include <vulkan/vulkan_core.h>

namespace the
{
  //This file will encompass all that is related to the resources found within VulkanEngine/lve_renderer.cpp
  //It makes more sense for them to be separate, rather than it all be a giant monolithic piece of code
  TheResources::TheResources(TheDevice& device) : theDevice{device}
  {
    pools.resize(4);
    layouts.resize(4);
    sets.resize(4);

    pools[0] = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(1)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 5)
            .build();

    pools[1] = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(1)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1001) //need to make sure that I will never have to worry about this
                                                                                                 //eventually
            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .build();

    pools[2] = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(1)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .build();

    pools[3] = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(20)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 400)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 400)
            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT)
            .build();

    layouts[0] = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, 1)
            .build();

    layouts[1] = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT, 20,
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            .addDescriptorFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)
            .build();

    layouts[2] = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT, 1000, 
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT, 1000, 
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            .addDescriptorFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)
            .build();

    initBaseTextures();
  }

  VkDescriptorImageInfo TheResources::descriptorImageInfoHelper(TheDevice& device, VkImageView imageView)
  {
    VkSampler sampler;
    TheTextures::createTextureSampler(device, sampler);
    VkDescriptorImageInfo descriptorInfo{};
    descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    descriptorInfo.imageView = imageView;
    descriptorInfo.sampler = sampler;
    return descriptorInfo;
  }

  void TheResources::initBaseTextures()
  {
    baseResources = {std::make_shared<TheTextures>(theDevice, "textures/NA.png", TheTextures::COLOR), std::make_shared<TheTextures>(theDevice, "textures/NAM.png", TheTextures::COLOR)};

    auto image1 = baseResources[0]->getDescriptorInfo();
    auto image2 = baseResources[1]->getDescriptorInfo();

    TheDescriptorWriter(*layouts[1], *pools[1])
      .addImage(0, &image1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0)
      .addImage(0, &image2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
      .build(sets[1]);
  }
};
