#pragma once

#include "the_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace the {

    class TheDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(TheDevice& theDevice) : theDevice{ theDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);

            Builder& addBindingFlag(
                     VkDescriptorBindingFlags bindingFlags,
                     uint32_t count = 1);

            std::unique_ptr<TheDescriptorSetLayout> build() const;

        private:
            TheDevice& theDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
            std::vector<VkDescriptorBindingFlags> bindingFlags{};
        };

        TheDescriptorSetLayout(
            TheDevice& theDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~TheDescriptorSetLayout(){}
        TheDescriptorSetLayout(const TheDescriptorSetLayout&) = delete;
        TheDescriptorSetLayout& operator=(const TheDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
        void createLayoutTextures();

    private:
        TheDevice& theDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
        std::vector<VkDescriptorBindingFlags> bindingFlags;

        friend class TheDescriptorWriter;
    };

    class TheDescriptorPool {
    public:
        class Builder {
        public:
            Builder(TheDevice& theDevice) : theDevice{ theDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<TheDescriptorPool> build() const;

        private:
            TheDevice& theDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            std::vector<std::pair<VkDescriptorType, uint32_t>> placehold;
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        TheDescriptorPool(
            TheDevice& theDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes,
            std::vector<std::pair<VkDescriptorType, uint32_t>> placehold);
        ~TheDescriptorPool();
        TheDescriptorPool(const TheDescriptorPool&) = delete;
        TheDescriptorPool& operator=(const TheDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor);

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
        bool expandPool(const VkDescriptorPoolCreateInfo &poolInfo);
        VkDescriptorPool getDescriptorPool() {return descriptorPool;}
        void resetPool();

    private:
        TheDevice& theDevice;
        VkDescriptorPool descriptorPool;
        uint32_t maxSets;
        VkDescriptorPoolCreateFlags poolFlags;
        std::vector<std::pair<VkDescriptorType, uint32_t>> sizes;

        friend class TheDescriptorWriter;
    };

    class TheDescriptorWriter {
    public:
        TheDescriptorWriter(TheDescriptorSetLayout& setLayout, TheDescriptorPool& pool);

        TheDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        TheDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
        TheDescriptorWriter& addImage(uint32_t binding, VkDescriptorImageInfo* imageInfo, uint32_t index);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        TheDescriptorSetLayout& setLayout;
        TheDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace the
