/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///STL
#include <mutex>

///PeachCore
#include "../Utils/Plugin.h"
#include "../Utils/ShaderUtils.h"
#include "../Utils/Serializer.h"
#include "../Utils/DynamicLoader.h"

//////////////////////////////////////////////
// Language Support
//////////////////////////////////////////////

#include "../Language-Support/DotnetRuntime.h"
#include "../Language-Support/LuaScriptRuntime.h"
#include "../Language-Support/PythonScriptComponent.h"
#include "../Language-Support/PythonScriptRuntime.h"

///External
#include <physfs.h>
#include <stb/stb_image.h>
#include <miniaudio/miniaudio.h>
#include <moody_camel/readerwriterqueue.h>

#include <sodium.h>

typedef Plugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(Plugin*);

namespace PeachCore {

    constexpr uint16_t RESOURCE_OP_RESOURCE_DESTINATION = 69; // ProcessComannds() expects a node ID after

    constexpr uint16_t RESOURCE_OP_LOAD_MP3 = 1000;
    constexpr uint16_t RESOURCE_OP_LOAD_FLAC = 1001;
    constexpr uint16_t RESOURCE_OP_LOAD_WAV = 1002;

    constexpr uint16_t RESOURCE_OP_LOAD_TEXTURE = 2000;
    constexpr uint16_t RESOURCE_OP_LOAD_MESH = 2001;
    constexpr uint16_t RESOURCE_OP_LOAD_ANIMATION = 2002;

    constexpr uint16_t RESOURCE_OP_LOAD_OPENGL_SHADER = 2003; //WARNING: not sure if i wanna do a generic shader load not sure how differntiating between the graphics pipeline steps should work here
    constexpr uint16_t RESOURCE_OP_LOAD_VULKAN_SHADER = 2004; //WARNING: not sure if i wanna do a generic shader load not sure how differntiating between the graphics pipeline steps should work here

    constexpr uint16_t RESOURCE_OP_LOAD_SCRIPT = 3000;
    constexpr uint16_t RESOURCE_OP_LOAD_BYTECODE = 3001;
    constexpr uint16_t RESOURCE_OP_LOAD_DOTNET_RUNTIME = 3002;

    constexpr uint16_t RESOURCE_OP_LOAD_PLUGIN = 3003;

    struct LoadCommand
    {
        uint64_t NodeID;       // 4 bytes
        uint16_t OP;        // 2 bytes
    };

    //////////////////////////////////////////////
    // Runtime Contexts
    //////////////////////////////////////////////

    struct LuaRuntimeContext
    {

    };

    struct PythonRuntimeContext
    {

    };

    //////////////////////////////////////////////
    // Data Containers
    //////////////////////////////////////////////

    struct PluginData
    {
        unique_ptr<Plugin, DestroyPluginFunc> Pwugin = { nullptr, nullptr }; //>O<
        DYNLIB_HANDLE Handle = nullptr; //>w<
    };

    // ResourcePackage.h
    struct TextureData
    {
        int Width, Height, Channels;
        // owns data via unique_ptr + custom deleter
        unique_ptr<unsigned char, void(*)(void*)> PixelData{ nullptr, stbi_image_free };

        TextureData(unsigned char* fp_RawData, int fp_Width, int fp_Height, int fp_Channels)
        {
            Width = fp_Width;
            Height = fp_Height;
            Channels = fp_Channels;
            PixelData = { fp_RawData, stbi_image_free };
        }
    };

    struct AudioData { /* ... */ };
    struct MeshData { /* ... */ };
    struct AnimationData { /* ... */ };
    struct BytecodeData {/* ... */ }; //bytecode used for C# or bongojam script or lua ig
    struct ScriptData {/* ... */ }; // large string for raw script
    
    enum class ResourceKind : uint8_t 
    {
        Texture,
        Audio,
        Mesh,
        Animation,
        Bytecode,
        Script,
        Plugin,
        INVALID
    };

    using ResourcePayload = variant<
        unique_ptr<TextureData>,
        unique_ptr<AudioData>,
        unique_ptr<MeshData>,
        unique_ptr<AnimationData> ,
        unique_ptr<BytecodeData>,
        unique_ptr<ScriptData>,
        unique_ptr<PluginData>
    >;

    struct ResourceTransfer
    {
        uint64_t NodeID = 0;   // who this is for
        ResourceKind Kind = ResourceKind::INVALID;

        ResourcePayload Payload;

        ~ResourceTransfer() = default;
        ResourceTransfer() = default;
    };


    //////////////////////////////////////////////
    // ResourceManager Class
    //////////////////////////////////////////////
    class ResourceManager 
    {
    //////////////////////////////////////////////
    // Private Destructor & Constructor
    //////////////////////////////////////////////
    private:
        ~ResourceManager() = default;
        ResourceManager() = default;

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static ResourceManager& get_single()
        {
            static ResourceManager resource_loader;
            return resource_loader;
        }

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

    private:
    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        //////////////////// Resource Transfer Queues ////////////////////

        //used to push loaded assets that are destined for AudioManager
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_AudioResourceLoadingQueue = nullptr;
        //used to push loaded assets that are destined for RenderingManager
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_DrawableResourceLoadingQueue = nullptr;
        //used to push loaded scripts and config stuff -> MainThread/GameManager
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_MainThreadLoadingQueue = nullptr;

        //////////////////// Load Command Queue ////////////////////

        //used for asking ResourceManager to load something from the main thread
        shared_ptr<moodycamel::ReaderWriterQueue<LoadCommand, TESTING_CAMEL_QUEUE_SIZE>> pm_LoadCommandQueue = nullptr;

        //////////////////// Waiting Buffer ////////////////////

        vector<ResourceTransfer> pm_WaitingResources;

        //////////////////// Resource Logger ////////////////////

        //Resource Logger owned by ResourceManager only
        unique_ptr<Logger> resource_logger = nullptr;

        //////////////////// Utility Types ////////////////////

        //1 byte bois UwU
        Serializer pm_Serializer; 
        DynamicLoader pm_DynamicLoader;

        //////////////////// Thread Initialization Safeguards ////////////////////

        mutex pm_InitializationMutex;
        condition_variable pm_InitializationCompleteCondition;
        bool pm_IsInitialized = false;

        //////////////////// ETC ////////////////////

        string pm_RootDirectory;
        vector<uint8_t> pm_PeachBinary; //holds the currently loaded peachbin file
        atomic<bool> pm_IsActive = true;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool
            Initialize
            (
                const string& fp_LogOutputDirectory,
                const string& fp_RootPhysfsDirectory
            );

        void
            WaitUntilInitialized();

        bool
            ResourceLoop
            (
                const string& fp_LogOutputDirectory,
                const string& fp_RootPhysfsDirectory
            );

        [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>>
            GetAudioResourceLoadingQueue();

        [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>>
            GetDrawableResourceLoadingQueue();

        [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<LoadCommand, TESTING_CAMEL_QUEUE_SIZE>>
            GetLoadCommandQueue();

        bool
            LoadLuaRuntime();

        bool
            LoadDotNetRuntime
            (
                const string& fp_HostFxrPath,
                DotnetContext& fp_DotnetContext
            );

        bool
            LoadPythonRuntime();

        bool
            LoadPlugin
            (
                const string& fp_PluginFilePath,
                PluginData& fp_Plugin
            )
            const;

        void
            ShutdownResourceManager()
        {
            pm_IsActive = false;
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        ////////////////////////////////////////////////
        // Directory Detection Functions
        ////////////////////////////////////////////////
        void
            CheckForDirectoryChanges();

        // Function to list all files recursively
        unordered_map<string, filesystem::file_time_type>
            GetCurrentDirectoryState
            (
                const filesystem::path& fp_Directory
            );

        bool
            CompareStates
            (
                const unordered_map<string, filesystem::file_time_type>& fp_OldState,
                const unordered_map <string, filesystem::file_time_type>& fp_NewState
            );

        void
            CheckAndUpdateFileSystem() //XXX: this function seems kinda sus idk if it works as i want it too lmfao
            ;

        ////////////////////////////////////////////////
        // Directory Detection Functions
        ////////////////////////////////////////////////

        void
            ProcessCommands();

        ////////////////////////////////////////////////
        // Resource Loading Functions
        ////////////////////////////////////////////////

        bool
            LoadDotNetScript(const string& fp_ScriptPath);

        bool
            LoadLuaScript(const string& fp_ScriptPath);

        bool
            LoadBongoJamScript(const string& fp_ScriptPath);

        bool 
            LoadTextureFromFile(const string& fp_TextureFilePath, const uint64_t fp_DestinationNode);

        bool
            LoadWavFromFile(const string& fp_WavFilePath);

        bool
            LoadCompiledShader(const string& fp_ShaderFilePath);

        bool
            LoadScene(const string& fp_ScenePath); //load scene locally from PhysFS

        bool
            LoadScene
            (
                uint64_t* fp_Offset
            ); //unpacked from peachbin file loaded into peach engine rn

        //bool 
        //    TryPushingLoadedTexture
        //    (
        //        const string& fp_ObjectID, 
        //        unique_ptr<TextureData> fp_TextureDataPtr
        //    );
    };
}