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
#include "ShaderCompilerUtils.h"

namespace PeachEditor::ShaderCompilerUtils { //static TU isolated functions

    [[nodiscard]] static shaderc_shader_kind
        GetShaderType(ShaderStage fp_ShaderType)
    {
        switch (fp_ShaderType)
        {
        case ShaderStage::Vertex: return shaderc_glsl_vertex_shader;
        case ShaderStage::Fragment: return shaderc_glsl_fragment_shader;
        case ShaderStage::Compute: return shaderc_glsl_compute_shader;
        case ShaderStage::Geometry: return shaderc_glsl_geometry_shader;
        case ShaderStage::TessControl: return shaderc_glsl_tess_control_shader;
        case ShaderStage::TessEval: return shaderc_glsl_tess_evaluation_shader;
        default: return shaderc_glsl_infer_from_source;
        }
    }
}

namespace PeachEditor::ShaderCompilerUtils {

    bool
        CompileGLSLToSPIRV
        (
            const string& fp_ShaderSourcePath,
            const string& fp_OutputPath,
            ShaderStage fp_ShaderStage,
            PeachCore::Logger* logger
        )
    {
        PEACH_TO_DO_UNUSED(fp_ShaderSourcePath);
        PEACH_TO_DO_UNUSED(fp_OutputPath);
        PEACH_TO_DO_UNUSED(fp_ShaderStage);

        logger->Error("CompileGLSLToSPIRV not yet implemented on this platform", "ShaderCompilerUtils");
        return false;
    }

    bool
        OptimizeSPIRV
        (
            const string& fp_InputSpvPath,
            const string& fp_OutputSpvPath,
            PeachCore::Logger* logger
        )
    {
        PEACH_TO_DO_UNUSED(fp_InputSpvPath);
        PEACH_TO_DO_UNUSED(fp_OutputSpvPath);

        logger->Error("OptimizeSPIRV not yet implemented on this platform", "ShaderCompilerUtils");
        return false;
    }

    bool
        ValidateSPIRV
        (
            const string& fp_SpvPath,
            PeachCore::Logger* logger
        )
    {
        PEACH_TO_DO_UNUSED(fp_SpvPath);

        logger->Error("ValidateSPIRV not yet implemented on this platform", "ShaderCompilerUtils");
        return false;
    }

    bool
        CrossCompileToMSL
        (
            const string& fp_SpvPath,
            const string& fp_OutputPath,
            PeachCore::Logger* logger
        )
    {
        PEACH_TO_DO_UNUSED(fp_SpvPath);
        PEACH_TO_DO_UNUSED(fp_OutputPath);

        logger->Error("CrossCompileToMSL not yet implemented on this platform", "ShaderCompilerUtils");
        return false;
    }

    bool
        CrossCompileToHLSL
    (
        const string& fp_SpvPath,
        const string& fp_OutputPath,
        PeachCore::Logger* logger
    )
    {
        PEACH_TO_DO_UNUSED(fp_SpvPath);
        PEACH_TO_DO_UNUSED(fp_OutputPath);

        logger->Error("CrossCompileToHLSL not yet implemented on this platform", "ShaderCompilerUtils");
        return false;
    }

    bool
        CompileFullShaderPipeline
    (
        const string& fp_ShaderSourcePath,
        const string& fp_OutputDirectory,
        ShaderStage fp_ShaderStage,
        PeachCore::Logger* logger
    )
    {
        PEACH_TO_DO_UNUSED(fp_ShaderSourcePath);
        PEACH_TO_DO_UNUSED(fp_OutputDirectory);
        PEACH_TO_DO_UNUSED(fp_ShaderStage);

        logger->Error("CompileFullShaderPipeline not yet implemented on this platform", "ShaderCompilerUtils");
        return false;
    }

    CompilationResult
        CompileShaderFromSource
        (
            const string& fp_RawSource,
            const ShaderStage fp_ShaderStage,
            const string& fp_OutputFileName,
            const vector<string>& fp_MacroDefinitions,
            const string& fp_EntryPoint,
            const bool fp_IsOptimized
        )
    {
        shaderc::Compiler f_Compiler;
        shaderc::CompileOptions f_CompilerConfig;

        for (const string& __str : fp_MacroDefinitions)
        {
            f_CompilerConfig.AddMacroDefinition(__str);
        }

        f_CompilerConfig.SetGenerateDebugInfo();

        if (fp_IsOptimized)
        {
            f_CompilerConfig.SetOptimizationLevel(shaderc_optimization_level_performance);
        }

        shaderc_shader_kind f_ShaderType = GetShaderType(fp_ShaderStage);

        shaderc::SpvCompilationResult f_CompiledResult =
            f_Compiler.CompileGlslToSpv
            (
                fp_RawSource,
                f_ShaderType,
                fp_OutputFileName.c_str(),
                fp_EntryPoint.c_str(),
                f_CompilerConfig
            );

        if (f_CompiledResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            return
            {
                {},
                f_CompiledResult.GetErrorMessage(),
                false
            };
        }

        return
        {
            { f_CompiledResult.cbegin(), f_CompiledResult.cend() },
            "",
            true
        };
    }

    bool
        DisassembleSPIRV
        (
            const vector<uint32_t>& fp_SpirvBytecode,
            string& fp_DisassembledSPIRV,
            PeachCore::Logger* logger
        )
    {
        if (fp_DisassembledSPIRV.size() > 0)
        {
            logger->Warning("Tried to pass non empty string, continuing disassembly and clearing string but please be aware this might cause unintended side affects owo", "ShaderCompilerUtils");
            fp_DisassembledSPIRV.clear();
        }

        spvtools::SpirvTools tools(SPV_ENV_VULKAN_1_2);

        tools.SetMessageConsumer
        (
            [](spv_message_level_t, const char*, const spv_position_t&, const char* message)
            {
                fprintf(stderr, "[SPIRV-Tools] %s\n", message); //ew fprintf
            }
        );

        string f_;

        if (not tools.Disassemble(fp_SpirvBytecode, &fp_DisassembledSPIRV))
        {
            logger->Error("[SPIRV-Tools] Failed to disassemble SPIR-V", "ShaderCompilerUtils");
            return false;
        }

        return true;
    }

    ShaderReflectionInfo
        ReflectInputsOutputs
        (
            const vector<uint32_t>& fp_SpirvBytecode
        )
    {
        auto compiler = make_unique<spirv_cross::CompilerGLSL>(fp_SpirvBytecode); //needa heap alloc these because they're chonky boyz
        auto resources = make_unique<spirv_cross::ShaderResources>(compiler->get_shader_resources());

        ShaderReflectionInfo f_ReflectedInfo;

        for (const auto& input : resources->stage_inputs)
        {
            f_ReflectedInfo.InputVars.push_back(input.name);
        }

        for (const auto& ubo : resources->uniform_buffers)
        {
            f_ReflectedInfo.UniformBuffers.push_back(ubo.name);
        }

        for (const auto& sampler : resources->sampled_images)
        {
            f_ReflectedInfo.SampledImages.push_back(sampler.name);
        }

        return f_ReflectedInfo;
    }

    bool
        ReflectDescriptorBindings
        (
            vector<PeachCore::Vulkan::DescriptorBindingInfo>* fp_BindingInfo,
            const vector<uint32_t>& fp_SpirvBytecode,
            PeachCore::Logger* logger
        )
    {
        //check for nullptrs
        if (not logger)
        {
            PEACH_PRINT_ERROR("Tried to pass nullptr reference to logger during ReflectDescriptorBindings(), nothing was done.");
            return false;
        }
        else if (not fp_BindingInfo)
        {
            logger->Error("Nullptr vector reference passed to ReflectDescriptorBindings(), nothing was done.", "ShaderCompilerUtils");
            return false;
        }
        else if (not fp_BindingInfo->empty()) //check if vector is not empty after validating it isn't a nullptr ref
        {
            logger->Error("Tried passing non empty vector to ReflectDescriptorBindings(), nothing was done.", "ShaderCompilerUtils");
            return false;
        }

        auto compiler = make_unique<spirv_cross::Compiler>(fp_SpirvBytecode);
        auto resources = make_unique<spirv_cross::ShaderResources>(compiler->get_shader_resources());

        auto ExtractBindings =
            [&](const auto& fp_ResourceList, VkDescriptorType fp_DescriptorType)
            {
                for (const auto& __res : fp_ResourceList)
                {
                    const auto& set = compiler->get_decoration(__res.id, spv::DecorationDescriptorSet);
                    const auto& binding = compiler->get_decoration(__res.id, spv::DecorationBinding);

                    fp_BindingInfo->emplace_back
                    (
                        __res.name,
                        binding,
                        set,
                        fp_DescriptorType
                    );
                }
            };

        ExtractBindings(resources->uniform_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        ExtractBindings(resources->sampled_images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        ExtractBindings(resources->storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
        ExtractBindings(resources->separate_images, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
        ExtractBindings(resources->separate_samplers, VK_DESCRIPTOR_TYPE_SAMPLER);

        return true;
    }

    bool
        ReflectPushConstants
        (
            vector<PeachCore::Vulkan::PushConstantInfo>* fp_PushConstants,
            const vector<uint32_t>& fp_SpirvBytecode,
            PeachCore::Logger* logger
        )
    {
        //check for nullptrs
        if (not logger)
        {
            PEACH_PRINT_ERROR("Tried to pass nullptr reference to logger during ReflectPushConstants(), nothing was done.");
            return false;
        }
        else if (not fp_PushConstants)
        {
            logger->Error("Nullptr vector reference passed to ReflectPushConstants(), nothing was done.", "ShaderCompilerUtils");
            return false;
        }
        else if (not fp_PushConstants->empty())
        {
            logger->Error("Tried passing non empty vector to ReflectPushConstants(), nothing was done.", "ShaderCompilerUtils");
            return false;
        }

        auto compiler = make_unique<spirv_cross::Compiler>(fp_SpirvBytecode);

        if (compiler->get_shader_resources().push_constant_buffers.empty())
        {
            logger->Error("Tried to get push constants from a SPIRV shader that doesn't contain any push constants, nothing was done.", "ShaderCompilerUtils");
            return false;
        }

        for (const auto& __rng : compiler->get_active_buffer_ranges(compiler->get_shader_resources().push_constant_buffers[0].id))
        {
            fp_PushConstants->emplace_back
            (
                compiler->get_name(__rng.index),
                static_cast<uint32_t>(__rng.offset),
                static_cast<uint32_t>(__rng.range),
                static_cast<VkShaderStageFlags>(compiler->get_execution_model() == spv::ExecutionModelFragment ? VK_SHADER_STAGE_FRAGMENT_BIT : VK_SHADER_STAGE_VERTEX_BIT)
            );
        }

        return true;
    }
}