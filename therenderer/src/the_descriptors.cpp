#include "../headers/the_descriptors.hpp"
#include "../../theloading/headers/the_textures.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace the {

    // *************** Descriptor Set Layout Builder *********************

    TheDescriptorSetLayout::Builder& TheDescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    //TODO: remove this hacky piece of shit and make something neater
    TheDescriptorSetLayout::Builder& TheDescriptorSetLayout::Builder::addBindingFlag(
                     VkDescriptorBindingFlags bindingFlags,
                     uint32_t count)
    {
        VkDescriptorBindingFlags flags{};
        flags = bindingFlags;
        this->bindingFlags.push_back(flags);

        return *this;
    }

    std::unique_ptr<TheDescriptorSetLayout> TheDescriptorSetLayout::Builder::build() const {
        return std::make_unique<TheDescriptorSetLayout>(theDevice, bindings);
    }

    // *************** Descriptor Set Layout *********************

    TheDescriptorSetLayout::TheDescriptorSetLayout(
        TheDevice& theDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : theDevice{ theDevice }, bindings{ bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (bindingFlags.size() > 0) 
        {
            VkDescriptorSetLayoutBindingFlagsCreateInfo flags{};
            flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
            flags.pBindingFlags = &bindingFlags[0];
            flags.bindingCount = 1;

            descriptorSetLayoutInfo.pNext = &flags;
        }

        if (vkCreateDescriptorSetLayout(
            theDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    // *************** Descriptor Pool Builder *********************

    TheDescriptorPool::Builder& TheDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) {
        poolSizes.push_back({ descriptorType, count });
        placehold.push_back({ descriptorType, count });
        return *this;
    }

    TheDescriptorPool::Builder& TheDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) {
        poolFlags = flags;
        return *this;
    }
    TheDescriptorPool::Builder& TheDescriptorPool::Builder::setMaxSets(uint32_t count) {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<TheDescriptorPool> TheDescriptorPool::Builder::build() const {
        return std::make_unique<TheDescriptorPool>(theDevice, maxSets, poolFlags, poolSizes, placehold);
    }

    // *************** Descriptor Pool *********************

    TheDescriptorPool::TheDescriptorPool(
        TheDevice& theDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes,
        std::vector<std::pair<VkDescriptorType, uint32_t>> placehold)
        : theDevice{ theDevice } {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;
        sizes = placehold; //to fix
        this->maxSets = maxSets;
        this->poolFlags = poolFlags;

        if (vkCreateDescriptorPool(theDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    TheDescriptorPool::~TheDescriptorPool() {
        vkDestroyDescriptorPool(theDevice.device(), descriptorPool, nullptr);
    }

    bool TheDescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        //update pool until descriptor count is correct
        while (vkAllocateDescriptorSets(theDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS)
        {
          //Get rid of the old, small, puny pool
          vkDestroyDescriptorPool(theDevice.device(), descriptorPool, nullptr);
          std::vector<VkDescriptorPoolSize> newSizes{};  //Not too happy with how I do this, buuuuuuuut so long as it works I am willing to let it exist, for now
          for(int i = 0; i < sizes.size(); i++)
          {
            //multiply the existing size by 2
            sizes[i].second *= 2;
            newSizes.push_back({sizes[i].first, sizes[i].second});
          }

          //Create a new pool
          VkDescriptorPoolCreateInfo descriptorPoolInfo{};
          descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
          descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(newSizes.size());
          descriptorPoolInfo.pPoolSizes = newSizes.data();
          descriptorPoolInfo.maxSets = maxSets;
          descriptorPoolInfo.flags = poolFlags;
          allocInfo.descriptorPool = descriptorPool;

          expandPool(descriptorPoolInfo);
          allocInfo.descriptorPool = descriptorPool;
        }
        return true;
    }

    bool TheDescriptorPool::expandPool(const VkDescriptorPoolCreateInfo &poolInfo)
    {
      return vkCreateDescriptorPool(theDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS;
    }

    void TheDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            theDevice.device(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void TheDescriptorPool::resetPool() {
        vkResetDescriptorPool(theDevice.device(), descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    TheDescriptorWriter::TheDescriptorWriter(TheDescriptorSetLayout& setLayout, TheDescriptorPool& pool)
        : setLayout{ setLayout }, pool{ pool } {}

    TheDescriptorWriter& TheDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    TheDescriptorWriter& TheDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    TheDescriptorWriter& TheDescriptorWriter::addImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo, uint32_t index) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        //assert(
        //    bindingDescription.descriptorCount == 1 &&
        //    "Binding single descriptor info, but binding expects multiple"); 

        VkWriteDescriptorSet write{};
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.dstArrayElement = index;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        writes.push_back(write);
        return *this;
    }

    bool TheDescriptorWriter::build(VkDescriptorSet& set) {
        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void TheDescriptorWriter::overwrite(VkDescriptorSet& set) {
        for (auto& write : writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool.theDevice.device(), writes.size(), writes.data(), 0, nullptr);
    }

}  // namespace the
