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
#include <semaphore>
#include <latch>

///PeachCore
//#include "../Rendering/VulkanShaderProgram.h"
#include "../Utils/Serializer.h"
#include "../Utils/DynamicLoader.h"
#include "../Utils/NullResources.h"

//////////////////////////////////////////////
// Language Support
//////////////////////////////////////////////

#include "../Language-Support/NativeScript.h"
#include "../Language-Support/DotnetRuntime.h"
#include "../Language-Support/LuaScriptRuntime.h"

///External
#include <physfs/physfs.h>
#include <miniaudio/miniaudio.h>
#include <moody_camel/readerwriterqueue.h>

typedef NativeScript* (*CreateNativeScriptFunc)();
typedef void (*DestroyNativeScriptFunc)(NativeScript*);

namespace PeachCore {

    //struct PeachResPath //used to convert OS directory paths -> res:// vfs paths uwu
    //{
    //    string ResourcePath;

    //    explicit
    //        PeachResPath()
    //};

    //for longer lived pure data sources, ID's are helpful for locating them on the gpu or PCM audio
    using AudioID = uint64_t;
    using TextureID = uint64_t;
    using MeshID = uint64_t;
    using AnimationID = uint64_t;

    using VulkanShaderBytecode = vector<uint32_t>;

    using MaxCountingSemaphore = std::counting_semaphore<std::counting_semaphore<>::max()>;

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

    struct LoadCommand
    {
        uint64_t NodeID = 0; //node destination, nodes are created first in the scenetree, then any resources tied to it are asked to be loaded from GameManager
        string Location; //either a res:// inside the .peachbin or a location validated by the user on their system
        bool IsExternal = false; //by default will look in .peachbin otherwise if true will look elsewhere ig
        RESOURCE_OP OP = RESOURCE_OP::INVALID_LOAD; // UwU!
    };

    //////////////////////////////////////////////
    // PeachBinary Data
    //////////////////////////////////////////////

    enum class PeachBinChapterType : uint8_t
    {
        INVALID,
        PNG_TEXTURE,
        JPEG_TEXTURE,
        OBJ_MESH,
        ANIMATION,
        MP3_AUDIO,
        WAV_AUDIO,
        FLAC_AUDIO,
        LUA_BYTECODE,
        SHADER_BYTECODE,
        SCENE
    };

    struct PeachBinSection //don't needa track offset here since PeachBinChapter can do that using ChunkSize from the vector
    {
        string ResPath;
        uint16_t NameSize;

        uint64_t ChunkSize;
        vector<uint8_t> BinaryData; // absolute offset from start of file //MAYBE: use a unique_ptr here to express ownership

        explicit
            PeachBinSection(const string& fp_ResPath, vector<uint8_t>&& fp_BinaryData)
        {
            if (fp_ResPath.size() > numeric_limits<uint16_t>::max())
            {
                throw length_error("PeachBinSection: ResPath too long for uint16_t NameSize");
            }

            ResPath = fp_ResPath;
            BinaryData = std::move(fp_BinaryData);
            ChunkSize = BinaryData.size();
            NameSize = static_cast<uint16_t>(fp_ResPath.size());
        }
    };

    struct PeachBinChapter
    {
        uint8_t Magic = 0;  //Magic Type Number // enum: 1=Textures, 2=Meshes, 3=Bytecode, 4=Audio, ...
        uint64_t TotalSize;      // Total size of asset chapter
        unordered_map<string, PeachBinSection> Contents;
    };

    struct NativeScriptData
    {
        unique_ptr<NativeScript, DestroyNativeScriptFunc> Instance = { nullptr, nullptr }; //>O<
        DYNLIB_HANDLE Handle = nullptr; //>w<
    };

    //////////////////////////////////////////////
    // Data Containers
    //////////////////////////////////////////////

    struct TextureData
    {
        int Width, Height, Channels;
        // owns data via unique_ptr + custom deleter
        unique_ptr<unsigned char, void(*)(void*)> PixelData{ nullptr, free };

        explicit
            TextureData
            (
                unsigned char* fp_RawData,
                int fp_Width,
                int fp_Height,
                int fp_Channels
            )
        {
            Width = fp_Width;
            Height = fp_Height;
            Channels = fp_Channels;
            PixelData = { fp_RawData, free };
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
        unique_ptr<NativeScriptData>,
        unique_ptr<SceneData>,
        unique_ptr<VulkanShaderBytecode>
    >;

    struct ResourceTransfer //just gonna use holds_alternative instead of a tagged union
    {
        uint64_t NodeID = 0;   // who this is for

        ResourcePayload Payload;

        ~ResourceTransfer() = default;

        ResourceTransfer(const uint64_t fp_NodeDestination, ResourcePayload&& fp_ResourcePayload)
        {
            NodeID = fp_NodeDestination;
            Payload = std::move(fp_ResourcePayload);
        }
    };

    using ResourcePipe = moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>;
    using LoadCommandPipe = moodycamel::ReaderWriterQueue<LoadCommand, MOODY_CAMEL_QUEUE_SIZE>;

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
        shared_ptr<ResourcePipe> pm_AudioResourceLoadingQueue = nullptr;
        //used to push loaded assets that are destined for RenderingManager
        shared_ptr<ResourcePipe> pm_DrawableResourceLoadingQueue = nullptr;
        //used to push loaded scripts and config stuff -> MainThread/GameManager
        shared_ptr<ResourcePipe> pm_MainThreadLoadingQueue = nullptr;

        //////////////////// Load Command Queue ////////////////////

        //used for asking ResourceManager to load something from the main thread
        shared_ptr<LoadCommandPipe> pm_LoadCommandQueue = nullptr;

        //////////////////// Resource Logger ////////////////////

        //Resource Logger owned by ResourceManager only
        unique_ptr<Logger> resource_logger = nullptr;

        //////////////////// Thread Initialization Safeguards ////////////////////

        bool pm_IsInitialized = false;

        //////////////////// Semaphore Control ////////////////////

        MaxCountingSemaphore pm_ResourceSemaphore{ 0 }; // starts locked (zero tickets)

        atomic<bool> pm_IsRunning = true;

        //////////////////// Binary Data ////////////////////

        using PeachBinary = unordered_map<string, uint64_t>; // res:// path : binary_offset

        PeachBinary pm_OffsetTable; // res:// : binary_offset
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

        [[nodiscard]] shared_ptr<ResourcePipe>
            GetAudioResourceLoadingQueue
            (
                Logger*const logger
            ); //this is supposed to be called from the audio thread so cant use the resource_logger here for thread reasons

        [[nodiscard]] shared_ptr<ResourcePipe>
            GetDrawableResourceLoadingQueue
            (
                Logger* const logger
            ); //this is supposed to be called from the render thread so cant use the resource_logger here for thread reasons

        [[nodiscard]] shared_ptr<LoadCommandPipe>
            GetLoadCommandQueue
            (
                Logger* const logger
            ); //this is supposed to be called from the main thread so cant use the resource_logger here for thread reasons

        bool
            LoadDotNetRuntime
            (
                const string& fp_HostFxrPath,
                DotnetContext& fp_DotnetContext
            );

        bool
            LoadNativeSciptInstanceFFS
            (
                const string& fp_PluginFilePath,
                NativeScriptData& fp_Plugin
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
            ProcessCommand(const LoadCommand& fp_Command);

        ////////////////////////////////////////////////
        // Resource Loading Functions
        ////////////////////////////////////////////////
        // FFS = From File System, FB = From Binary
        [[nodiscard]] bool
            LoadPeachBinHeader(const string& fp_BinaryPath);

        bool
            LoadDotNetScript(const string& fp_ScriptPath);

        bool
            LoadLuaBytecodeFB(const string& fp_ResPath);

        bool
            LoadBongoJamScript(const string& fp_ScriptPath);

        bool 
            LoadTextureFFS
            (
                const string& fp_FilePath, 
                const uint64_t fp_DestinationNode
            );

        bool
            LoadTextureFB
            (
                const string& fp_ResPath, 
                const uint64_t fp_DestinationNode
            );

        bool
            LoadWavFFS(const string& fp_WavFilePath);

        bool
            LoadVulkanShaderFFS
            (
                const string& fp_ShaderFilePath,
                const uint64_t fp_DestinationNode
            );

        bool
            LoadOpenGLShaderFFS
            (
                const string& fp_ShaderFilePath,
                const uint64_t fp_DestinationNode
            );

        bool
            LoadSceneFFS(const string& fp_FilePath); //load scene locally from PhysFS

        bool
            LoadSceneFB(const string& fp_ResPath); //unpacked from peachbin file loaded into peach engine rn
    };
}