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

///PeachCore
#include "../Utils/Plugin.h"
#include "../Utils/ShaderUtils.h"
#include "../Utils/Serializer.h"
#include "../Utils/DynamicLoader.h"

#include "../Language-Support/DotnetRuntime.h"

///External
#include <physfs.h>
#include <stb/stb_image.h>
#include <miniaudio/miniaudio.h>
#include <moody_camel/readerwriterqueue.h>

typedef Plugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(Plugin*);

namespace PeachCore {

    //////////////////////////////////////////////
    // Plugin Stuff
    //////////////////////////////////////////////
    struct PluginInfo
    {
        unique_ptr<Plugin, DestroyPluginFunc> Pwugin = { nullptr, nullptr }; //>O<
        DYNLIB_HANDLE Handle = nullptr; //>w<
    };

    struct LuaRuntimeContext
    {

    };

    struct PythonRuntimeContext
    {

    };

    //////////////////////////////////////////////
    // ResourceManager word size
    //////////////////////////////////////////////

    struct LoadCommand 
    {
        uint64_t node_id;       // 4 bytes
        uint16_t opcode;        // 2 bytes
        uint16_t reserved;      // 2 bytes (alignment or flags)
        uint64_t operand;       // 8 bytes
    };

    struct ResourceTransfer 
    {
        uint64_t NodeID = 0;
        //ResourceType type; // enum: Texture, Audio, Mesh, etc.
        //ResourceHandle handle;
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
        //////////////////// Queue Pointers ////////////////////

        //used to push loaded assets that are destined for AudioManager
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_AudioResourceLoadingQueue = nullptr;
        //used to push loaded assets that are destined for RenderingManager
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_DrawableResourceLoadingQueue = nullptr;
        //used to push loaded scripts and config stuff -> MainThread/GameManager
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_MainThreadLoadingQueue = nullptr;

        //used for asking ResourceManager to load something from the main thread
        shared_ptr<moodycamel::ReaderWriterQueue<LoadCommand, TESTING_CAMEL_QUEUE_SIZE>> pm_LoadCommandQueue = nullptr;

        //////////////////// Waiting Buffers ////////////////////

        // Holds mesh, texture, shader and animation data
        vector<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_WaitingLoadedGraphicsAssets;
        //Holds mp3, wav and flac files
        vector<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_WaitingLoadedAudioAssets;

        //////////////////// Resource Logger ////////////////////

        //Resource Logger owned by ResourceManager only
        unique_ptr<Logger> resource_logger = nullptr;

        //////////////////// Utility Structs ////////////////////

        //1 byte bois UwU
        Serializer pm_Serializer; 
        DynamicLoader pm_DynamicLoader;

        //////////////////// Script Runtime Contexts ////////////////////

        LuaRuntimeContext pm_LuaRuntimeContext;
        PythonRuntimeContext pm_PythonRuntimeContext;

        string pm_RootDirectory;

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        atomic<bool> m_IsInitialized = false;
        atomic<bool> m_IsActive = false;

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
            CheckForDirectoryChanges();

        // Function to list all files recursively
        unordered_map<string, filesystem::file_time_type>
            GetCurrentDirectoryState
            (
                const filesystem::path& fp_Directory
            );

        ////////////////////////////////////////////////
// Directory Detection Functions
////////////////////////////////////////////////

        bool
            CompareStates
            (
                const unordered_map<string, filesystem::file_time_type>& fp_OldState,
                const unordered_map <string, filesystem::file_time_type>& fp_NewState
            );

        void
            CheckAndUpdateFileSystem() //XXX: this function seems kinda sus idk if it works as i want it too lmfao
            ;

        bool
            ResourceLoop();

        void
            ProcessCommands();

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
            LoadDotNetScript(const string& fp_ScriptPath);

        bool
            LoadPythonRuntime();

        bool
            LoadPlugin
            (
                const string& fp_PluginFilePath,
                PluginInfo& fp_Plugin
            )
            const;

        bool 
            LoadTextureFromFile(const string& fp_TextureFilePath);

        bool
            LoadWavFromFile(const string& fp_WavFilePath);

        bool
            LoadCompiledShader(const string& fp_ShaderFilePath);

        bool
            LoadScene(const string& fp_ScenePath); //load scene locally from PhysFS

        bool
            LoadScene
            (
                const vector<uint8_t>& fp_SceneData, //current loaded binary
                uint64_t* fp_Offset
            ); //unpacked from peachbin file loaded into peach engine rn

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        //bool 
        //    TryPushingLoadedTexture
        //    (
        //        const string& fp_ObjectID, 
        //        unique_ptr<TextureData> fp_TextureDataPtr
        //    );
    };
}