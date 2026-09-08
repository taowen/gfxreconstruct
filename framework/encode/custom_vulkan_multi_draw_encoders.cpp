/*
** Copyright (c) 2018-2020 Valve Corporation
** Copyright (c) 2018-2020 LunarG, Inc.
** Copyright (c) 2023 Advanced Micro Devices, Inc. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
*/

#include "encode/custom_vulkan_api_call_encoders.h"
#include "encode/custom_vulkan_encoder_commands.h"
#include "encode/parameter_encoder.h"
#include "encode/vulkan_capture_manager.h"
#include "encode/vulkan_handle_wrapper_util.h"
#include "generated/generated_vulkan_struct_encoders.h"

#include <cstddef>
#include <cstring>

GFXRECON_BEGIN_NAMESPACE(gfxrecon)
GFXRECON_BEGIN_NAMESPACE(encode)

VKAPI_ATTR void VKAPI_CALL vkCmdDrawMultiEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    drawCount,
    const VkMultiDrawInfoEXT*                   pVertexInfo,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    uint32_t                                    stride)
{
    VulkanCaptureManager* manager = VulkanCaptureManager::Get();
    GFXRECON_ASSERT(manager != nullptr);
    auto force_command_serialization = manager->GetForceCommandSerialization();
    std::shared_lock<CommonCaptureManager::ApiCallMutexT> shared_api_call_lock;
    std::unique_lock<CommonCaptureManager::ApiCallMutexT> exclusive_api_call_lock;
    if (force_command_serialization)
    {
        exclusive_api_call_lock = VulkanCaptureManager::AcquireExclusiveApiCallLock();
    }
    else
    {
        shared_api_call_lock = VulkanCaptureManager::AcquireSharedApiCallLock();
    }

    CustomEncoderPreCall<format::ApiCallId::ApiCall_vkCmdDrawMultiEXT>::Dispatch(manager, commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);

    auto encoder = manager->BeginTrackedApiCallCapture(format::ApiCallId::ApiCall_vkCmdDrawMultiEXT);
    if (encoder)
    {
        encoder->EncodeVulkanHandleValue<vulkan_wrappers::CommandBufferWrapper>(commandBuffer);
        encoder->EncodeUInt32Value(drawCount);
        // Store logical draws contiguously; replay consumes a compact decoded array.
        // Keep the original pointer and stride for the application driver call.
        encoder->EncodeStructArrayPreamble(pVertexInfo, drawCount);
        if (pVertexInfo != nullptr)
        {
            for (size_t i = 0; i < drawCount; ++i)
            {
                const auto* bytes = reinterpret_cast<const uint8_t*>(pVertexInfo) + i * stride;
                VkMultiDrawInfoEXT info{};
                std::memcpy(&info, bytes, sizeof(info));
                EncodeStruct(encoder, info);
            }
        }
        encoder->EncodeUInt32Value(instanceCount);
        encoder->EncodeUInt32Value(firstInstance);
        encoder->EncodeUInt32Value(sizeof(VkMultiDrawInfoEXT));
        manager->EndCommandApiCallCapture(commandBuffer);
    }

    vulkan_wrappers::GetDeviceTable(commandBuffer)->CmdDrawMultiEXT(commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);

    CustomEncoderPostCall<format::ApiCallId::ApiCall_vkCmdDrawMultiEXT>::Dispatch(manager, commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);

}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawMultiIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    drawCount,
    const VkMultiDrawIndexedInfoEXT*            pIndexInfo,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    uint32_t                                    stride,
    const int32_t*                              pVertexOffset)
{
    VulkanCaptureManager* manager = VulkanCaptureManager::Get();
    GFXRECON_ASSERT(manager != nullptr);
    auto force_command_serialization = manager->GetForceCommandSerialization();
    std::shared_lock<CommonCaptureManager::ApiCallMutexT> shared_api_call_lock;
    std::unique_lock<CommonCaptureManager::ApiCallMutexT> exclusive_api_call_lock;
    if (force_command_serialization)
    {
        exclusive_api_call_lock = VulkanCaptureManager::AcquireExclusiveApiCallLock();
    }
    else
    {
        shared_api_call_lock = VulkanCaptureManager::AcquireSharedApiCallLock();
    }

    CustomEncoderPreCall<format::ApiCallId::ApiCall_vkCmdDrawMultiIndexedEXT>::Dispatch(manager, commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);

    auto encoder = manager->BeginTrackedApiCallCapture(format::ApiCallId::ApiCall_vkCmdDrawMultiIndexedEXT);
    if (encoder)
    {
        encoder->EncodeVulkanHandleValue<vulkan_wrappers::CommandBufferWrapper>(commandBuffer);
        encoder->EncodeUInt32Value(drawCount);
        // Store logical draws contiguously; replay consumes a compact decoded array.
        // Keep the original pointer and stride for the application driver call.
        encoder->EncodeStructArrayPreamble(pIndexInfo, drawCount);
        if (pIndexInfo != nullptr)
        {
            for (size_t i = 0; i < drawCount; ++i)
            {
                const auto* bytes = reinterpret_cast<const uint8_t*>(pIndexInfo) + i * stride;
                VkMultiDrawIndexedInfoEXT info{};
                std::memcpy(&info.firstIndex, bytes, sizeof(info.firstIndex));
                std::memcpy(&info.indexCount, bytes + sizeof(info.firstIndex), sizeof(info.indexCount));
                // With a shared offset the per-draw field need not be readable.
                if (pVertexOffset == nullptr)
                {
                    std::memcpy(&info.vertexOffset, bytes + offsetof(VkMultiDrawIndexedInfoEXT, vertexOffset), sizeof(info.vertexOffset));
                }
                EncodeStruct(encoder, info);
            }
        }
        encoder->EncodeUInt32Value(instanceCount);
        encoder->EncodeUInt32Value(firstInstance);
        encoder->EncodeUInt32Value(sizeof(VkMultiDrawIndexedInfoEXT));
        encoder->EncodeInt32Ptr(pVertexOffset);
        manager->EndCommandApiCallCapture(commandBuffer);
    }

    vulkan_wrappers::GetDeviceTable(commandBuffer)->CmdDrawMultiIndexedEXT(commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);

    CustomEncoderPostCall<format::ApiCallId::ApiCall_vkCmdDrawMultiIndexedEXT>::Dispatch(manager, commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);

}

GFXRECON_END_NAMESPACE(encode)
GFXRECON_END_NAMESPACE(gfxrecon)
