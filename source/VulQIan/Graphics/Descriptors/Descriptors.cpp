// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "../../Exception/Exception.hpp"
#include "Descriptors.hpp"

#include <cassert>
#include <stdexcept>

namespace Vulqian::Engine::Graphics::Descriptors {
// *************** Descriptor Set Layout Builder *********************

DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::addBinding(
    uint32_t           binding,
    VkDescriptorType   descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t           count) {
    assert(bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
    return std::make_unique<DescriptorSetLayout>(this->device, bindings);
}

// *************** Descriptor Set Layout *********************

DescriptorSetLayout::DescriptorSetLayout(
    Device &Device, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings)
    : device{Device}, bindings{bindings} {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto const& [key, value] : bindings) {
        setLayoutBindings.push_back(value);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(
            this->device.get_device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("descriptor set layout");
    }
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(this->device.get_device(), descriptorSetLayout, nullptr);
}

// *************** Descriptor Pool Builder *********************

DescriptorPool::Builder &DescriptorPool::Builder::addPoolSize(
    VkDescriptorType descriptorType, uint32_t count) {
    poolSizes.push_back({descriptorType, count});
    return *this;
}

DescriptorPool::Builder &DescriptorPool::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags) {
    poolFlags = flags;
    return *this;
}
DescriptorPool::Builder &DescriptorPool::Builder::setMaxSets(uint32_t count) {
    maxSets = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
    return std::make_unique<DescriptorPool>(this->device, this->maxSets, this->poolFlags, this->poolSizes);
}

// *************** Descriptor Pool *********************

DescriptorPool::DescriptorPool(
    Device                                  &device,
    uint32_t                                 maxSets,
    VkDescriptorPoolCreateFlags              poolFlags,
    const std::vector<VkDescriptorPoolSize> &poolSizes)
    : device{device} {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(this->device.get_device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
        VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("descriptor set pool");
    }
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(this->device.get_device(), this->descriptorPool, nullptr);
}

bool DescriptorPool::allocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(this->device.get_device(), &allocInfo, &descriptor) != VK_SUCCESS) {
        return false;
    }
    return true;
}

void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const {
    vkFreeDescriptorSets(
        this->device.get_device(),
        this->descriptorPool,
        static_cast<uint32_t>(descriptors.size()),
        descriptors.data());
}

void DescriptorPool::resetPool() {
    vkResetDescriptorPool(this->device.get_device(), this->descriptorPool, 0);
}

// *************** Descriptor Writer *********************

DescriptorWriter::DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool)
    : setLayout{setLayout}, pool{pool} {}

DescriptorWriter &DescriptorWriter::writeBuffer(
    uint32_t binding, const VkDescriptorBufferInfo* bufferInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto const &bindingDescription{setLayout.bindings[binding]};

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

DescriptorWriter &DescriptorWriter::writeImage(
    uint32_t binding, const VkDescriptorImageInfo* imageInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto const &bindingDescription{setLayout.bindings[binding]};

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

bool DescriptorWriter::build(VkDescriptorSet &set) {
    if (bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set); !success) {
        return false;
    }
    overwrite(set);
    return true;
}

void DescriptorWriter::overwrite(VkDescriptorSet &set) {
    for (auto &write : writes) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool.device.get_device(), writes.size(), writes.data(), 0, nullptr);
}
}  // namespace Vulqian::Engine::Graphics::Descriptors
