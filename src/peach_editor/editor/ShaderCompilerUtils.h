/*******************************************************************
 *                     Peach Editor v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#pragma once

///Vulkan
#include <shaderc/shaderc.hpp>
#include <spirv-tools/libspirv.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

///PeachCore
#include <debug/Logger.h>
#include <rendering/Vulkan/PipelineFactory.h>

///STL
#include <string>

namespace PeachEditor::ShaderCompilerUtils
{
    using namespace std;

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

    [[nodiscard]] bool
        CompileGLSLToSPIRV
        (
            const string& fp_ShaderSourcePath,
            const string& fp_OutputPath,
            ShaderStage fp_ShaderStage,
            PeachCore::Logger* logger
        );

    [[nodiscard]] bool
        OptimizeSPIRV
        (
            const string& fp_InputSpvPath,
            const string& fp_OutputSpvPath,
            PeachCore::Logger* logger
        );

    // validate SPIRV using spirv-val
    [[nodiscard]] bool
        ValidateSPIRV
        (
            const string& fp_SpvPath,
            PeachCore::Logger* logger
        );

    [[nodiscard]] bool
        CrossCompileToMSL
        (
            const string& fp_SpvPath,
            const string& fp_OutputPath,
            PeachCore::Logger* logger
        );

    [[nodiscard]] bool
        CrossCompileToHLSL
        (
            const string& fp_SpvPath,
            const string& fp_OutputPath,
            PeachCore::Logger* logger
        );

    // runs the full pipeline, compile -> validate -> optimize
    [[nodiscard]] bool
        CompileFullShaderPipeline
        (
            const string& fp_ShaderSourcePath,
            const string& fp_OutputDirectory,
            ShaderStage fp_ShaderStage,
            PeachCore::Logger* logger
        );

    [[nodiscard]] CompilationResult
        CompileShaderFromSource
        (
            const string& fp_RawSource,
            const ShaderStage fp_ShaderStage,
            const string& fp_OutputFileName,
            const vector<string>& fp_MacroDefinitions,
            const string& fp_EntryPoint = "main",
            const bool fp_IsOptimized = true
        );

    [[nodiscard]] bool
        DisassembleSPIRV
        (
            const vector<uint32_t>& fp_SpirvBytecode,
            string& fp_DisassembledSPIRV,
            PeachCore::Logger* logger
        );

    [[nodiscard]] ShaderReflectionInfo
        ReflectInputsOutputs(const vector<uint32_t>& fp_SpirvBytecode);

    bool
        ReflectDescriptorBindings
        (
            vector<PeachCore::Vulkan::DescriptorBindingInfo>* fp_BindingInfo,
            const vector<uint32_t>& fp_SpirvBytecode,
            PeachCore::Logger* logger
        );

     bool
        ReflectPushConstants
        (
            vector<PeachCore::Vulkan::PushConstantInfo>* fp_PushConstants,
            const vector<uint32_t>& fp_SpirvBytecode,
            PeachCore::Logger* logger
        );
}
