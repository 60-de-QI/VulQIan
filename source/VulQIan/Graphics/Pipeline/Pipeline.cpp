// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "Pipeline.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>

#include "../../Exception/Exception.hpp"
namespace Vulqian::Engine::Graphics {

Pipeline::Pipeline(
    Vulqian::Engine::Graphics::Device& device,
    const std::string& vert_filepath,
    const std::string& frag_filepath,
    const PipelineConstructInfo& config) : device(device) {
    this->create_graphics_pipeline(vert_filepath, frag_filepath, config);
}

Pipeline::~Pipeline() {
    vkDestroyShaderModule(this->device.device(), this->vert_module, nullptr);
    vkDestroyShaderModule(this->device.device(), this->frag_module, nullptr);
    vkDestroyPipeline(this->device.device(), this->graphics_pipeline, nullptr);
}

std::vector<char> Pipeline::read_file(const std::string& path) {
    std::ifstream file{path, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        throw Vulqian::Exception::failed_to_open("file at " + path);
    }

    auto file_size = static_cast<size_t>(file.tellg());

    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();

    return buffer;
}

void Pipeline::create_graphics_pipeline(const std::string& vert_filepath, const std::string& frag_filepath, const PipelineConstructInfo& config) {
    assert(config.pipeline_layout != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no pipeline_layout provided in config.");
    assert(config.render_pass != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no render_pass provided in config.");

    auto vert_code = this->read_file(vert_filepath);
    auto frag_code = this->read_file(frag_filepath);

    this->create_shader_module(vert_code, &this->vert_module);
    this->create_shader_module(frag_code, &this->frag_module);

    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stage;
    shader_stage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stage[0].module = this->vert_module;
    shader_stage[0].pName = "main";
    shader_stage[0].flags = 0;
    shader_stage[0].pNext = nullptr;
    shader_stage[0].pSpecializationInfo = nullptr;

    shader_stage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stage[1].module = this->frag_module;
    shader_stage[1].pName = "main";
    shader_stage[1].flags = 0;
    shader_stage[1].pNext = nullptr;
    shader_stage[1].pSpecializationInfo = nullptr;

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr;
    vertex_input_info.pVertexBindingDescriptions = nullptr;

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stage.begin();
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &config.input_assembly_info;
    pipeline_info.pViewportState = &config.viewport_info;
    pipeline_info.pRasterizationState = &config.rasterization_info;
    pipeline_info.pMultisampleState = &config.multisample_info;
    pipeline_info.pColorBlendState = &config.color_blend_info;
    pipeline_info.pDepthStencilState = &config.depth_stencil_info;
    pipeline_info.pDynamicState = nullptr;

    pipeline_info.layout = config.pipeline_layout;
    pipeline_info.renderPass = config.render_pass;
    pipeline_info.subpass = config.subpass;

    pipeline_info.basePipelineIndex = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("graphics pipeline");
    }
}

void Pipeline::create_shader_module(const std::vector<char>& code, VkShaderModule* shader_mod) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = std::bit_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(this->device.device(), &create_info, nullptr, shader_mod) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("shader module");
    }
}

void Pipeline::get_default_config(PipelineConstructInfo default_conf, uint32_t width, uint32_t height) noexcept {
    // Represents the first step of the pipeline // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPrimitiveTopology.html
    default_conf.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    default_conf.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  // Every three verteces are grouped in triangles // A strip would use the previous two verteces with a new one to form a triangle
    default_conf.input_assembly_info.primitiveRestartEnable = VK_FALSE;               // When this to true when using a strip we can break up a strip by index value in a buffer

    // Describes the transformation between the image and the pipeline output {-1 to 1} view grid
    default_conf.viewport.x = 0.0f;
    default_conf.viewport.y = 0.0f;
    default_conf.viewport.width = static_cast<float>(width);
    default_conf.viewport.height = static_cast<float>(height);
    // Depthrange for the view port, will linearly transforme the Z position of shaders and such
    default_conf.viewport.minDepth = 0.0f;
    default_conf.viewport.maxDepth = 1.0f;

    // Anyting outside the scissor limits will be cut from the image
    default_conf.scissor.offset = {0, 0};
    default_conf.scissor.extent = {width, height};

    default_conf.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    default_conf.viewport_info.viewportCount = 1;
    default_conf.viewport_info.pViewports = &default_conf.viewport;
    default_conf.viewport_info.scissorCount = 1;
    default_conf.viewport_info.pScissors = &default_conf.scissor;

    // The rasterization stage: breaks up goematry in fragments for every pixel of the triangles that overlaps.
    default_conf.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    default_conf.rasterization_info.depthClampEnable = VK_FALSE;  // Clamp values between one or zero.
    default_conf.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    default_conf.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    default_conf.rasterization_info.lineWidth = 1.0f;
    default_conf.rasterization_info.cullMode = VK_CULL_MODE_NONE;
    default_conf.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    default_conf.rasterization_info.depthBiasEnable = VK_FALSE;
    default_conf.rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
    default_conf.rasterization_info.depthBiasClamp = 0.0f;           // Optional
    default_conf.rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

    // Multisampling stage, it's how edges of triangles are managed to avoid Edges (MSAA for ex is multisampling method)
    default_conf.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    default_conf.multisample_info.sampleShadingEnable = VK_FALSE;
    default_conf.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    default_conf.multisample_info.minSampleShading = 1.0f;           // Optional
    default_conf.multisample_info.pSampleMask = nullptr;             // Optional
    default_conf.multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
    default_conf.multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

    // Colour Blending is to control how colours mix together when triangles overlap
    default_conf.color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    default_conf.color_blend_attachment.blendEnable = VK_FALSE;
    default_conf.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    default_conf.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    default_conf.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    default_conf.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    default_conf.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    default_conf.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    default_conf.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    default_conf.color_blend_info.logicOpEnable = VK_FALSE;
    default_conf.color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
    default_conf.color_blend_info.attachmentCount = 1;
    default_conf.color_blend_info.pAttachments = &default_conf.color_blend_attachment;
    default_conf.color_blend_info.blendConstants[0] = 0.0f;  // Optional
    default_conf.color_blend_info.blendConstants[1] = 0.0f;  // Optional
    default_conf.color_blend_info.blendConstants[2] = 0.0f;  // Optional
    default_conf.color_blend_info.blendConstants[3] = 0.0f;  // Optional

    default_conf.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    default_conf.depth_stencil_info.depthTestEnable = VK_TRUE;
    default_conf.depth_stencil_info.depthWriteEnable = VK_TRUE;
    default_conf.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    default_conf.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    default_conf.depth_stencil_info.minDepthBounds = 0.0f;  // Optional
    default_conf.depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
    default_conf.depth_stencil_info.stencilTestEnable = VK_FALSE;
    default_conf.depth_stencil_info.front = {};  // Optional
    default_conf.depth_stencil_info.back = {};   // Optional
}

void Pipeline::bind(VkCommandBuffer command_buffer) {
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphics_pipeline);
}

}  // namespace Vulqian::Engine::Graphics
