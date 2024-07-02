// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "Device.hpp"

#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

#include "../../Exception/Exception.hpp"

namespace Vulqian::Engine::Graphics {
// local callback functions
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, [[maybe_unused]] void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

// class member functions
Device::Device(Vulqian::Engine::Window& window) : window{window} {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
}

Device::~Device() {
    vkDestroyCommandPool(this->device, this->command_pool, nullptr);
    vkDestroyDevice(this->device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(this->instance, this->debug_messenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, this->surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void Device::createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw Vulqian::Exception::unavailable("validation layers");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulQIan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VulQIan";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(this->validation_layers.size());
        createInfo.ppEnabledLayerNames = this->validation_layers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("vk instance");
    }

    hasGflwRequiredInstanceExtensions();
}

void Device::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw Vulqian::Exception::failed_to_find("GPUs with Vulkan support");
    }
    std::cout << "Device count: " << deviceCount << std::endl;
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& it_device : devices) {
        if (isDeviceSuitable(it_device)) {
            this->physical_device = it_device;
            break;
        }
    }

    if (this->physical_device == VK_NULL_HANDLE) {
        throw Vulqian::Exception::failed_to_find("a suitable GPU");
    }

    vkGetPhysicalDeviceProperties(this->physical_device, &properties);
    std::cout << "physical device: " << properties.deviceName << std::endl;
}

void Device::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(this->physical_device);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t>                   uniqueQueueFamilies = {indices.graphics_family, indices.present_family};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(this->device_extensions.size());
    createInfo.ppEnabledExtensionNames = this->device_extensions.data();

    // might not really be necessary anymore because device specific validation layers
    // have been deprecated
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(this->validation_layers.size());
        createInfo.ppEnabledLayerNames = this->validation_layers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(this->physical_device, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("logical device");
    }

    vkGetDeviceQueue(device, indices.graphics_family, 0, &graphics_queue);
    vkGetDeviceQueue(device, indices.present_family, 0, &present_queue);
}

void Device::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_family;
    poolInfo.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &command_pool) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("command pool");
    }
}

void Device::createSurface() {
    window.create_window_surface(instance, &this->surface);
}

bool Device::isDeviceSuitable(VkPhysicalDevice selected_device) {
    QueueFamilyIndices indices = findQueueFamilies(selected_device);

    bool extensionsSupported = checkDeviceExtensionSupport(selected_device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(selected_device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.present_modes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(selected_device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}

void Device::setupDebugMessenger() {
    if (!enableValidationLayers)
        return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debug_messenger) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_setup("debug messenger");
    }
}

bool Device::checkValidationLayerSupport() const {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : validation_layers) {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> Device::getRequiredExtensions() const {
    uint32_t     glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void Device::hasGflwRequiredInstanceExtensions() const {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "available extensions:" << std::endl;
    std::unordered_set<std::string> available;
    for (const auto& extension : extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
        available.emplace(extension.extensionName);
    }

    std::cout << "required extensions:" << std::endl;
    auto requiredExtensions = getRequiredExtensions();
    for (const auto& required : requiredExtensions) {
        std::cout << "\t" << required << std::endl;
        if (!available.contains(required)) {
            throw Vulqian::Exception::missing_requirements("glfw extension");
        }
    }
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice selected_device) const {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(selected_device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        selected_device,
        nullptr,
        &extensionCount,
        availableExtensions.data());

    std::set<std::string> requiredExtensions(this->device_extensions.cbegin(), this->device_extensions.cend());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice selected_device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
            indices.graphics_family_has_value = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(selected_device, i, this->surface, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.present_family = i;
            indices.present_family_has_value = true;
        }
        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice selected_device) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(selected_device, this->surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(selected_device, this->surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(selected_device, this->surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(selected_device, this->surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.present_modes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            selected_device,
            this->surface,
            &presentModeCount,
            details.present_modes.data());
    }
    return details;
}

VkFormat Device::findSupportedFormat(
    const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(this->physical_device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (
            tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw Vulqian::Exception::failed_to_find("supported format");
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(this->physical_device, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }
    throw Vulqian::Exception::failed_to_find("suitable memory type");
}

void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags_property, VkBuffer& buffer, VkDeviceMemory& buffer_memory) {
    // We specify the reqs for the buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(this->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("vertex buffer");
    }

    // Then we use these requirements and assign them to our buffer
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(this->device, buffer, &mem_requirements);

    // Then we allocate the memory for our buffer
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = mem_requirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, flags_property);

    if (vkAllocateMemory(this->device, &allocInfo, nullptr, &buffer_memory) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_allocate("vertex buffer memory");
    }

    vkBindBufferMemory(this->device, buffer, buffer_memory, 0);
}

VkCommandBuffer Device::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = this->command_pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(this->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(this->graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(this->graphics_queue);

    vkFreeCommandBuffers(this->device, this->command_pool, 1, &commandBuffer);
}

void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void Device::copyBufferToImage(
    VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layerCount;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
    endSingleTimeCommands(commandBuffer);
}

void Device::createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags flag_properties, VkImage& image, VkDeviceMemory& imageMemory) {
    if (vkCreateImage(this->device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("image");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(this->device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, flag_properties);

    if (vkAllocateMemory(this->device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_allocate("image memory");
    }

    if (vkBindImageMemory(this->device, image, imageMemory, 0) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_bind("image memory");
    }
}
} // namespace Vulqian::Engine::Graphics
