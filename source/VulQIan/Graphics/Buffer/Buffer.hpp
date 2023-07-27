// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Device/Device.hpp"

namespace Vulqian::Engine::Graphics {

class Buffer {
  public:
    Buffer(
        Vulqian::Engine::Graphics::Device& device,
        VkDeviceSize                       instanceSize,
        uint32_t                           instanceCount,
        VkBufferUsageFlags                 usageFlags,
        VkMemoryPropertyFlags              memoryPropertyFlags,
        VkDeviceSize                       minOffsetAlignment = 1);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void     unmap();

    void                   writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult               flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult               invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void                   writeToIndex(void* data, int index);
    VkResult               flushIndex(int index);
    VkDescriptorBufferInfo descriptorInfoForIndex(int index);
    VkResult               invalidateIndex(int index);

    VkBuffer              getBuffer() const noexcept { return buffer; }
    void*                 getMappedMemory() const noexcept { return mapped; }
    uint32_t              getInstanceCount() const noexcept { return instance_count; }
    VkDeviceSize          getInstanceSize() const noexcept { return instance_size; }
    VkDeviceSize          getAlignmentSize() const noexcept { return instance_size; }
    VkBufferUsageFlags    getUsageFlags() const noexcept { return usage_flags; }
    VkMemoryPropertyFlags getMemoryPropertyFlags() const noexcept { return memoryPropertyFlags; }
    VkDeviceSize          getBufferSize() const noexcept { return buffer_size; }

  private:
    Vulqian::Engine::Graphics::Device& vq_device;
    static VkDeviceSize get_alignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment);

    void*                              mapped = nullptr;
    VkBuffer                           buffer = VK_NULL_HANDLE;
    VkDeviceMemory                     memory = VK_NULL_HANDLE;

    VkDeviceSize          buffer_size;
    uint32_t              instance_count;
    VkDeviceSize          instance_size;
    VkDeviceSize          alignment_size;
    VkBufferUsageFlags    usage_flags;
    VkMemoryPropertyFlags memoryPropertyFlags;
};

} // namespace Vulqian::Engine::Graphics
