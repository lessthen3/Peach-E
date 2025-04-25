#include "../../../include/Peach-Core/Rendering/ShaderUtils.h"

namespace PeachCore {

    VkShaderModule
        ShaderUtils::CreateShaderModule
        (
            VkDevice device,
            const vector<uint32_t>& bytecode
        )
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytecode.size() * sizeof(uint32_t);
        createInfo.pCode = bytecode.data();

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw runtime_error("Failed to create shader module!");
        }

        return shaderModule;
    }
}