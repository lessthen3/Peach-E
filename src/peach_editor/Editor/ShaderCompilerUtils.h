/*******************************************************************
 *                     Peach Editor v0.0.7
 *             Created by Ranyodh Mandur - 🍑 2024
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#pragma once

///Vulkan
#include <shaderc.hpp>
#include <libspirv.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>
#include <vulkan/vulkan_core.h>

///PeachCore
#include <Utils/Logger.h>

///STL
#include <unordered_map>
#include <optional>

namespace PeachEditor
{
    using namespace std;

    class ShaderCompilerUtils
    {
    public:
        enum class ShaderStage
        {
            Vertex,
            Fragment,
            Compute,
            Geometry,
            TessControl,
            TessEval
        };

        struct CompilationResult
        {
            vector<uint32_t> Bytecode;
            string ErrorMessage;
            bool IsSuccess = false;
        };

        struct ShaderReflectionInfo 
        {
            vector<string> InputVars;
            vector<string> UniformBuffers;
            vector<string> SampledImages;
        };

        struct DescriptorBindingInfo 
        {
            string Name;
            uint32_t Binding;
            uint32_t Set;
            VkDescriptorType Type;
        };

        struct PushConstantInfo 
        {
            string Name;
            uint32_t Offset;
            uint32_t Size;
            VkShaderStageFlags StageFlags;
        };

    public:
        static CompilationResult
            CompileShaderFromSource
            (
                const string& fp_RawSource,
                const ShaderStage fp_ShaderStage,
                const string& fp_OutputFileName,
                const vector<string>& fp_MacroDefinitions,
                const string& fp_EntryPoint = "main",
                const bool fp_IsOptimized = true
            );

        static void
            DisassembleSPIRV
            (
                const vector<uint32_t>& fp_SpirvBytecode
            );

        static ShaderReflectionInfo
            ReflectInputsOutputs(const vector<uint32_t>& fp_SpirvBytecode);

        static bool
            ReflectDescriptorBindings
            (
                vector<DescriptorBindingInfo>* fp_BindingInfo,
                const vector<uint32_t>& fp_SpirvBytecode,
                PeachCore::Logger* logger
            );

        static bool
            ReflectPushConstants
            (
                vector<PushConstantInfo>* fp_PushConstants,
                const vector<uint32_t>& fp_SpirvBytecode,
                PeachCore::Logger* logger
            );

    private:
        static shaderc_shader_kind
            GetShaderType(ShaderStage fp_ShaderType);

        static bool
            WriteSPIRVToFile
            (
                const vector<uint32_t>& fp_SpirvBytecode,
                const string& fp_DesiredOutputDirectory,
                const string& fp_DesiredName,
                PeachCore::Logger* logger
            );

        static bool
            LoadRawShaderSource
            (
                string* fp_SourceCode,
                const string& fp_ShaderSourcePath,
                PeachCore::Logger* logger
            );
    };
}
