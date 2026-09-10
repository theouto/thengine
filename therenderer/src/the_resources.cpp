#include "../headers/the_resources.hpp"
#include <vulkan/vulkan_core.h>

namespace the
{
  //This file will encompass all that is related to the resources found within VulkanEngine/lve_renderer.cpp
  //It makes more sense for them to be separate, rather than it all be a giant monolithic piece of code
  TheResources::TheResources(TheDevice& device) : theDevice{device}
  {
    pools.resize(4);
    layouts.resize(3);

    pools[0] = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(1)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 5)
            .build();

    pools[1] = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(1)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1001) //need to make sure that I will never have to worry about this
            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .build();

    pools[2] = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(1)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4)
            .build();

    pools[3] = TheDescriptorPool::Builder(theDevice)
            .setMaxSets(20)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 400)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 400)
            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT)
            .build();

    layouts[0] = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1)
            .build();

    layouts[1] = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 20, 
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            .addDescriptorFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)
            .build();
    
    layouts[2] = TheDescriptorSetLayout::Builder(theDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1000, 
                        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            .addDescriptorFlags(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)
            .build();
  }

  void TheResources::generateDescriptors()
{
  globalSetLayouts.resize(TheSwapChain::MAX_FRAMES_IN_FLIGHT);

  auto nerd1 = textures[0]->getDescriptorInfo();
  auto nerd2 = textures[1]->getDescriptorInfo();

  LveDescriptorWriter(*bindlessSetLayout, *descriptorPool)
    .addImage(0, &nerd1, 0)
    .addImage(0, &nerd2, 1)
    .build(bindlessLayout);

  auto shadow = getShadowInfo(0);

  LveDescriptorWriter(*shadowSetLayout, *shadowPool)
      .addImage(0, &shadow, 0)
      .build(_shadowSet);

  for (int i = 1; i < TheSwapChain::SHADOW_CASCADES; i++)
  {
    shadow = getShadowInfo(i);

    LveDescriptorWriter(*shadowSetLayout, *shadowPool)
      .addImage(0, &shadow, i)
      .overwrite(_shadowSet);
  }

  for(int i = 0; i < TheSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
  {
    auto bufferInfo = getUboInfo(i);

    LveDescriptorWriter(*globalSetLayout, *globalPool)
      .writeBuffer(0, &bufferInfo)
      .build(globalSetLayouts[i]);
  }
}

void LveRenderer::updateDescriptors()
{
  for(int i = 0; i < TheSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
  {
    auto bufferInfo = getUboInfo(i);
    auto depthInfo = getDepthInfo();
    auto normalSpecInfo = getNormalInfo();

    auto renderInfo = getImages(i);

    TheDescriptorWriter(*globalSetLayout, *globalPool)
      .writeBuffer(0, &bufferInfo)
      .writeImage(2, &depthInfo)
      .writeImage(3, &normalSpecInfo)
      .overwrite(globalSetLayouts[i]);
  }

  for (int i = 0; i < LveSwapChain::SHADOW_CASCADES; i++)
  {
    auto shadowInfo = getShadowInfo(i);

    TheDescriptorWriter(*shadowSetLayout, *shadowPool)
      .addImage(0, &shadowInfo, i)
      .overwrite(_shadowSet);
  }
}

  VkDescriptorImageInfo TheResources::descriptorImageInfoHelper(TheDevice& device, VkImageView imageView)
  {
    VkSampler sampler;
    TheTextures::createTextureSampler(device, sampler);
    VkDescriptorImageInfo descriptorInfo{};
    descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
    descriptorInfo.imageView = imageView;
    descriptorInfo.sampler = sampler;
    return descriptorInfo;
  }
};
