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
#include <semaphore>
#include <latch>

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

///External
#include <physfs.h>
#include <stb/stb_image.h>
#include <miniaudio/miniaudio.h>
#include <moody_camel/readerwriterqueue.h>

#include <sodium.h>

typedef Plugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(Plugin*);

namespace PeachCore {

    enum class RESOURCE_OP : uint16_t
    {
        RESOURCE_DESTINATION = 69,

        LOAD_MP3 = 1000,
        LOAD_FLAC = 1001,
        LOAD_WAV = 1002,

        LOAD_TEXTURE = 2000,
        LOAD_MESH = 2001,
        LOAD_ANIMATION = 2002,
        LOAD_OPENGL_SHADER = 2003,
        LOAD_VULKAN_SHADER = 2004,

        LOAD_SCRIPT = 3000,
        LOAD_BYTECODE = 3001,
        LOAD_DOTNET_RUNTIME = 3002,
        LOAD_PLUGIN = 3003,

        LOAD_SCENE = 4000,
        LOAD_PREFAB = 4001,

        INVALID_LOAD
    };

    struct PeachBinaryAsset
    {
        uint64_t Offset = 0;
        uint64_t Size = 0;
    };

    using ResourceLocation = variant<PeachBinaryAsset, string>;

    struct LoadCommand
    {
        uint64_t NodeID = 0;       //node destination, nodes are created first in the scenetree, then any resources tied to it are asked to be loaded from GameManager
        RESOURCE_OP OP = RESOURCE_OP::INVALID_LOAD;        // 2 bytes
        ResourceLocation Location;
    };

    //////////////////////////////////////////////
    // Runtime Contexts
    //////////////////////////////////////////////

    struct LuaRuntimeContext
    {

    };

    struct BongoJamRuntimeContext
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

    struct SceneData
    {

    };

    using ResourcePayload = variant<
        unique_ptr<TextureData>,
        unique_ptr<AudioData>,
        unique_ptr<MeshData>,
        unique_ptr<AnimationData> ,
        unique_ptr<BytecodeData>,
        unique_ptr<ScriptData>,
        unique_ptr<PluginData>,
        unique_ptr<SceneData>
    >;

    struct ResourceTransfer //just gonna use holds_alternative instead of a tagged union
    {
        uint64_t NodeID = 0;   // who this is for

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
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>> pm_AudioResourceLoadingQueue = nullptr;
        //used to push loaded assets that are destined for RenderingManager
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>> pm_DrawableResourceLoadingQueue = nullptr;
        //used to push loaded scripts and config stuff -> MainThread/GameManager
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>> pm_MainThreadLoadingQueue = nullptr;

        //////////////////// Load Command Queue ////////////////////

        //used for asking ResourceManager to load something from the main thread
        shared_ptr<moodycamel::ReaderWriterQueue<LoadCommand, MOODY_CAMEL_QUEUE_SIZE>> pm_LoadCommandQueue = nullptr;

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

        bool pm_IsInitialized = false;

        //////////////////// Semaphore Control ////////////////////

        binary_semaphore pm_ResourceSemaphore{ 0 }; // starts locked (zero tickets)
        atomic<bool> pm_IsRunning = true;

        //////////////////// Binary Data ////////////////////

        vector<uint8_t> pm_PeachMetaBinary; //holds the meta file that 
        vector<uint8_t> pm_PeachBinary;


        //////////////////// Directory Information ////////////////////

        string pm_RootDirectory;

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
            ResourceLoop
            (
                const string& fp_LogOutputDirectory,
                const string& fp_RootPhysfsDirectory,
               latch& fp_InitLatch
            );

        [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>>
            GetAudioResourceLoadingQueue
            (
                Logger*const logger
            ); //this is supposed to be called from the audio thread so cant use the resource_logger here for thread reasons

        [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>>
            GetDrawableResourceLoadingQueue
            (
                Logger* const logger
            ); //this is supposed to be called from the render thread so cant use the resource_logger here for thread reasons

        [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<LoadCommand, MOODY_CAMEL_QUEUE_SIZE>>
            GetLoadCommandQueue
            (
                Logger* const logger
            ); //this is supposed to be called from the main thread so cant use the resource_logger here for thread reasons

        bool
            LoadLuaRuntime();

        bool
            LoadDotNetRuntime
            (
                const string& fp_HostFxrPath,
                DotnetContext& fp_DotnetContext
            );

        bool
            LoadPlugin
            (
                const string& fp_PluginFilePath,
                PluginData& fp_Plugin
            )
            const;

        void
            Stop()
        {
            pm_IsRunning.store(false, std::memory_order_release);
            pm_ResourceSemaphore.release(); // Wake it up to exit        
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
            LoadTexture(const string& fp_TextureFilePath, const uint64_t fp_DestinationNode);

        bool
            LoadTexture(const uint64_t* fp_TextureFilePath, const uint64_t fp_DestinationNode);

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

        void 
            TryPushingResourceTransfer
            (
               ResourceTransfer&& fp_ResourceTransfer
            );
    };
}