// /*******************************************************************
//  *                     Peach Editor v0.0.7
//  *             Created by Ranyodh Mandur - 🍑 2024
//  *
//  *             Licensed under the MIT License (MIT).
//  *         For more details, see the LICENSE file or visit:
//  *             https://opensource.org/licenses/MIT
//  *
//  *      Peach Editor is a free open source editor for Peach-E
// ********************************************************************/
// #include "ShaderCompilerUtils.h"

// namespace PeachEditor {

//     ShaderCompilerUtils::CompilationResult
//         ShaderCompilerUtils::CompileShaderFromSource
//         (
//             const string& fp_RawSource,
//             const ShaderStage fp_ShaderStage,
//             const string& fp_OutputFileName,
//             const vector<string>& fp_MacroDefinitions,
//             const string& fp_EntryPoint,
//             const bool fp_IsOptimized
//         )
//     {
//         shaderc::Compiler f_Compiler;
//         shaderc::CompileOptions f_CompilerConfig;

//         for (const string& __str : fp_MacroDefinitions)
//         {
//             f_CompilerConfig.AddMacroDefinition(__str);
//         }

//         f_CompilerConfig.SetGenerateDebugInfo();

//         if (fp_IsOptimized)
//         {
//             f_CompilerConfig.SetOptimizationLevel(shaderc_optimization_level_performance);
//         }

//         shaderc_shader_kind f_ShaderType = GetShaderType(fp_ShaderStage);

//         shaderc::SpvCompilationResult f_CompiledResult =
//             f_Compiler.CompileGlslToSpv
//             (
//                 fp_RawSource,
//                 f_ShaderType,
//                 fp_OutputFileName.c_str(),
//                 fp_EntryPoint.c_str(),
//                 f_CompilerConfig
//             );

//         if (f_CompiledResult.GetCompilationStatus() != shaderc_compilation_status_success)
//         {
//             return
//             {
//                 {},
//                 f_CompiledResult.GetErrorMessage(),
//                 false
//             };
//         }

//         return
//         {
//             { f_CompiledResult.cbegin(), f_CompiledResult.cend() },
//             "",
//             true
//         };
//     }

//      void
//         ShaderCompilerUtils::DisassembleSPIRV
//         (
//             const vector<uint32_t>& fp_SpirvBytecode
//         )
//     {
//         spvtools::SpirvTools tools(SPV_ENV_VULKAN_1_2);

//         tools.SetMessageConsumer
//         (
//             [](spv_message_level_t, const char*, const spv_position_t&, const char* message)
//             {
//                 fprintf(stderr, "[SPIRV-Tools] %s\n", message); //ew fprintf
//             }
//         );

//         string disassembled;

//         if (tools.Disassemble(fp_SpirvBytecode, &disassembled))
//         {
//             printf("--- SPIR-V Disassembly ---\n%s\n", disassembled.c_str());
//         }
//         else
//         {
//             fprintf(stderr, "[SPIRV-Tools] Failed to disassemble SPIR-V\n");
//         }
//     }

//      ShaderCompilerUtils::ShaderReflectionInfo
//         ShaderCompilerUtils::ReflectInputsOutputs(const vector<uint32_t>& fp_SpirvBytecode)
//     {
//         auto compiler = make_unique<spirv_cross::CompilerGLSL>(fp_SpirvBytecode); //needa heap alloc these because they're chonky boyz
//         auto resources = make_unique<spirv_cross::ShaderResources>(compiler->get_shader_resources());

//         ShaderReflectionInfo f_ReflectedInfo;

//         for (const auto& input : resources->stage_inputs)
//         {
//             f_ReflectedInfo.InputVars.push_back(input.name);
//         }

//         for (const auto& ubo : resources->uniform_buffers)
//         {
//             f_ReflectedInfo.UniformBuffers.push_back(ubo.name);
//         }

//         for (const auto& sampler : resources->sampled_images)
//         {
//             f_ReflectedInfo.SampledImages.push_back(sampler.name);
//         }

//         return f_ReflectedInfo;
//     }

//      bool
//         ShaderCompilerUtils::ReflectDescriptorBindings
//         (
//             vector<DescriptorBindingInfo>* fp_BindingInfo,
//             const vector<uint32_t>& fp_SpirvBytecode,
//             PeachCore::Logger* logger
//         )
//     {
//         //check for nullptrs
//         if (not logger)
//         {
//             PeachCore::PrintError("Tried to pass nullptr reference to logger during ReflectDescriptorBindings(), nothing was done.");
//             return false;
//         }
//         else if (not fp_BindingInfo)
//         {
//             logger->Error("Nullptr vector reference passed to ReflectDescriptorBindings(), nothing was done.", "ShaderCompilerUtils");
//             return false;
//         }
//         else if (not fp_BindingInfo->empty()) //check if vector is not empty after validating it isn't a nullptr ref
//         {
//             logger->Error("Tried passing non empty vector to ReflectDescriptorBindings(), nothing was done.", "ShaderCompilerUtils");
//             return false;
//         }

//         auto compiler = make_unique<spirv_cross::Compiler>(fp_SpirvBytecode);
//         auto resources = make_unique<spirv_cross::ShaderResources>(compiler->get_shader_resources());

//         auto ExtractBindings =
//             [&](const auto& fp_ResourceList, VkDescriptorType fp_DescriptorType)
//             {
//                 for (const auto& __res : fp_ResourceList)
//                 {
//                     const auto& set = compiler->get_decoration(__res.id, spv::DecorationDescriptorSet);
//                     const auto& binding = compiler->get_decoration(__res.id, spv::DecorationBinding);

//                     fp_BindingInfo->emplace_back
//                     (
//                         __res.name,
//                         binding,
//                         set,
//                         fp_DescriptorType
//                     );
//                 }
//             };

//         ExtractBindings(resources->uniform_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
//         ExtractBindings(resources->sampled_images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
//         ExtractBindings(resources->storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
//         ExtractBindings(resources->separate_images, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
//         ExtractBindings(resources->separate_samplers, VK_DESCRIPTOR_TYPE_SAMPLER);

//         return true;
//     }

//      bool
//         ShaderCompilerUtils::ReflectPushConstants
//         (
//             vector<PushConstantInfo>* fp_PushConstants,
//             const vector<uint32_t>& fp_SpirvBytecode,
//             PeachCore::Logger* logger
//         )
//     {
//         //check for nullptrs
//         if (not logger)
//         {
//             PeachCore::PrintError("Tried to pass nullptr reference to logger during ReflectPushConstants(), nothing was done.");
//             return false;
//         }
//         else if (not fp_PushConstants)
//         {
//             logger->Error("Nullptr vector reference passed to ReflectPushConstants(), nothing was done.", "ShaderCompilerUtils");
//             return false;
//         }
//         else if (not fp_PushConstants->empty())
//         {
//             logger->Error("Tried passing non empty vector to ReflectPushConstants(), nothing was done.", "ShaderCompilerUtils");
//             return false;
//         }

//         auto compiler = make_unique<spirv_cross::Compiler>(fp_SpirvBytecode);

//         if (compiler->get_shader_resources().push_constant_buffers.empty())
//         {
//             logger->Error("Tried to get push constants from a SPIRV shader that doesn't contain any push constants, nothing was done.", "ShaderCompilerUtils");
//             return false;
//         }

//         for (const auto& __rng : compiler->get_active_buffer_ranges(compiler->get_shader_resources().push_constant_buffers[0].id))
//         {
//             fp_PushConstants->emplace_back
//             (
//                 compiler->get_name(__rng.index),
//                 static_cast<uint32_t>(__rng.offset),
//                 static_cast<uint32_t>(__rng.range),
//                 static_cast<VkShaderStageFlags>(compiler->get_execution_model() == spv::ExecutionModelFragment ? VK_SHADER_STAGE_FRAGMENT_BIT : VK_SHADER_STAGE_VERTEX_BIT)
//             );
//         }

//         return true;
//     }

//      shaderc_shader_kind
//         ShaderCompilerUtils::GetShaderType(ShaderStage fp_ShaderType)
//     {
//         switch (fp_ShaderType)
//         {
//         case ShaderStage::Vertex: return shaderc_glsl_vertex_shader;
//         case ShaderStage::Fragment: return shaderc_glsl_fragment_shader;
//         case ShaderStage::Compute: return shaderc_glsl_compute_shader;
//         case ShaderStage::Geometry: return shaderc_glsl_geometry_shader;
//         case ShaderStage::TessControl: return shaderc_glsl_tess_control_shader;
//         case ShaderStage::TessEval: return shaderc_glsl_tess_evaluation_shader;
//         default: return shaderc_glsl_infer_from_source;
//         }
//     }

//      bool
//         ShaderCompilerUtils::WriteSPIRVToFile
//         (
//             const vector<uint32_t>& fp_SpirvBytecode,
//             const string& fp_DesiredOutputDirectory,
//             const string& fp_DesiredName,
//             PeachCore::Logger* logger
//         )
//     {
//         //always check for nullptrs kids >O<
//         if (not logger)
//         {
//             PeachCore::PrintError("ShaderCompilerUtils Error: Tried to pass nullptr reference to logger during WriteSPIRVToFile()");
//             return false;
//         }
//         // Ensure directory exists
//         else if (not filesystem::exists(fp_DesiredOutputDirectory))
//         {
//             logger->Error(format("ShaderCompilerUtils Error: Tried to pass invalid write directory: '{}' to WriteSPIRVToFile()", fp_DesiredOutputDirectory), "ShaderCompilerUtils");
//             return false;
//         }
//         else if (fp_SpirvBytecode.empty()) //check if the byte vector is empty uwu
//         {
//             logger->Error(format("ShaderCompilerUtils Error: Tried passing empty byte vector for writing to file name: '{}', nothing was done.", fp_DesiredName), "ShaderCompilerUtils");
//             return false;
//         }

//         const string f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName + ".spv";

//         ofstream file(f_FileName, ios::binary);  // Open in binary mode

//         if (not file.is_open())
//         {
//             logger->Error(format("ShaderCompilerUtils Error: Failed to open file: '{}' for writing.", f_FileName), "ShaderCompilerUtils");
//             return false;
//         }

//         //now we can confidently write the bytecode into a file knowing its open, the vector is filled and the directory exists
//         file.write(reinterpret_cast<const char*>(fp_SpirvBytecode.data()), fp_SpirvBytecode.size() * sizeof(uint32_t));

//         return true;
//     }

//      bool
//         ShaderCompilerUtils::LoadRawShaderSource
//         (
//             string* fp_SourceCode,
//             const string& fp_ShaderSourcePath,
//             PeachCore::Logger* logger
//         )
//     {
//         //check for nullptr for logger ref
//         if (not logger)
//         {
//             PeachCore::PrintError("ShaderCompilerUtils Error: Tried to pass nullptr reference to logger during LoadRawShaderSource()");
//             return false;
//         }
//         //more nullptr checking
//         else if (not fp_SourceCode)
//         {
//             logger->Error("ShaderCompilerUtils Error: Nullptr string reference passed to LoadRawShaderSource()", "ShaderCompilerUtils");
//             return false;
//         }
//         // Ensure directory exists
//         else if (not filesystem::exists(fp_ShaderSourcePath))
//         {
//             logger->Error("ShaderCompilerUtils Error: Tried to pass invalid filepath to LoadRawShaderSource()", "ShaderCompilerUtils");
//             return false;
//         }

//         // Extract file extension assuming format "filename.ext"
//         size_t lastDotIndex = fp_ShaderSourcePath.rfind('.');

//         if (lastDotIndex == string::npos)
//         {
//             logger->Error(format("ShaderCompilerUtils Error: No file extension found at filepath: '{}'", fp_ShaderSourcePath), "ShaderCompilerUtils");
//             return false;
//         }

//         string f_FileExtension = fp_ShaderSourcePath.substr(lastDotIndex);

//         if (
//             f_FileExtension != ".fs" and
//             f_FileExtension != ".vs" and
//             f_FileExtension != ".glsl" and
//             f_FileExtension != ".vert" and
//             f_FileExtension != ".frag"
//             )
//         {
//             logger->Error(format("Found file extension: '{}', when GLSL Shader was expected at specified filepath: '{}'", f_FileExtension, fp_ShaderSourcePath), "ShaderCompilerUtils");
//             return false;
//         }

//         ifstream f_ShaderFile(fp_ShaderSourcePath, ios::in);

//         if (not f_ShaderFile.is_open())
//         {
//             logger->Error(format("ShaderCompilerUtils Error: Failed to open shader at filepath: '{}', for reading.", fp_ShaderSourcePath), "ShaderCompilerUtils");
//             return false;
//         }

//         stringstream f_TempStringBuffer;
//         f_TempStringBuffer << f_ShaderFile.rdbuf();
//         *fp_SourceCode = f_TempStringBuffer.str();

//         return true;
//     }
// }