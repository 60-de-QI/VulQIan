// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "Buffer.hpp"

#include <cassert>
#include <cstring>
#include <ranges>

namespace Vulqian::Engine::Graphics {

/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param this->instance_size The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
VkDeviceSize Buffer::get_alignment(VkDeviceSize instance_size, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instance_size + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instance_size;
}

Buffer::Buffer(
    Vulqian::Engine::Graphics::Device& device,
    VkDeviceSize                       instanceSize,
    uint32_t                           instanceCount,
    VkBufferUsageFlags                 usageFlags,
    VkMemoryPropertyFlags              memoryPropertyFlags,
    VkDeviceSize                       minOffsetAlignment)
    : vq_device{device},
      instance_size{instanceSize},
      instance_count{instanceCount},
      usage_flags{usageFlags},
      memoryPropertyFlags{memoryPropertyFlags} {
    this->alignment_size = this->get_alignment(this->instance_size, minOffsetAlignment);
    this->buffer_size = this->alignment_size * instanceCount;
    this->vq_device.createBuffer(this->buffer_size, usageFlags, memoryPropertyFlags, buffer, memory);
}

Buffer::~Buffer() {
    unmap();
    vkDestroyBuffer(this->vq_device.get_device(), buffer, nullptr);
    vkFreeMemory(this->vq_device.get_device(), memory, nullptr);
}

/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    assert(buffer && memory && "Called map on buffer before create");
    return vkMapMemory(this->vq_device.get_device(), memory, offset, size, 0, &mapped);
}

/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
void Buffer::unmap() {
    if (mapped) {
        vkUnmapMemory(this->vq_device.get_device(), memory);
        mapped = nullptr;
    }
}

/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
void Buffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
    assert(this->mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        std::ranges::copy(static_cast<char*>(data), static_cast<char*>(data) + this->buffer_size, static_cast<char*>(this->mapped));
    } else {
        char* memOffset = static_cast<char*>(this->mapped) + offset;
        std::ranges::copy_n(static_cast<char*>(data), size, memOffset);
    }
}

/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */
VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mapped_range = {};
    mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mapped_range.memory = memory;
    mapped_range.offset = offset;
    mapped_range.size = size;
    return vkFlushMappedMemoryRanges(this->vq_device.get_device(), 1, &mapped_range);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mapped_range = {};
    mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mapped_range.memory = memory;
    mapped_range.offset = offset;
    mapped_range.size = size;
    return vkInvalidateMappedMemoryRanges(this->vq_device.get_device(), 1, &mapped_range);
}

/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */
VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
        buffer,
        offset,
        size,
    };
}

/**
 * Copies "this->instance_size" bytes of data to the mapped buffer at an offset of index * this->alignment_size
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
void Buffer::writeToIndex(void* data, int index) {
    writeToBuffer(data, this->instance_size, index * this->alignment_size);
}

/**
 *  Flush the memory range at index * this->alignment_size of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */
VkResult Buffer::flushIndex(int index) { return flush(this->alignment_size, index * this->alignment_size); }

/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * this->alignment_size
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
    return descriptorInfo(this->alignment_size, index * this->alignment_size);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * this->alignment_size
 *
 * @return VkResult of the invalidate call
 */
VkResult Buffer::invalidateIndex(int index) {
    return invalidate(this->alignment_size, index * this->alignment_size);
}

} // namespace Vulqian::Engine::Graphics
