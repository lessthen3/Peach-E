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
#include "../../include/Peach-Core/Managers/ResourceManager.h"

namespace PeachCore {

    bool
        ResourceManager::Initialize
        (
            const string& fp_LogOutputDirectory,
            const string& fp_RootPhysfsDirectory
        )
    {
        //////////////////// Resource Logger Initialization ////////////////////

        resource_logger = make_unique<Logger>();
        resource_logger->Initialize(ThreadName::ResourceThread, fp_LogOutputDirectory, "ResourceThread", Logger::LogLevel::ALL_LOGS);
        resource_logger->Debug("ResourceThreadLogger successfully initialized", "ResourceManager");

        //////////////////// Initialize Queues ////////////////////

        pm_AudioResourceLoadingQueue = make_shared<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>>();
        pm_DrawableResourceLoadingQueue = make_shared<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>>();
        pm_MainThreadLoadingQueue = make_shared<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>>();

        pm_LoadCommandQueue = make_shared<moodycamel::ReaderWriterQueue<LoadCommand, MOODY_CAMEL_QUEUE_SIZE>>();

        //////////////////// Set Executable Root Directory ////////////////////

        pm_RootDirectory = fp_RootPhysfsDirectory;

        resource_logger->Info("ResourceManager successfully initialized the all queues", "ResourceManager");

        pm_IsInitialized = true;

        return true;
    }

    void
        ResourceManager::ResourceLoop
        (
            const string& fp_LogOutputDirectory,
            const string& fp_RootPhysfsDirectory,
            latch& fp_InitLatch
        )
    {
        if (not Initialize(fp_LogOutputDirectory, fp_RootPhysfsDirectory))
        {

            return;
        }

        fp_InitLatch.count_down(); //count down latch should be the resource latch inside gamemanager uwu

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            // Block until main thread wakes us
            pm_ResourceSemaphore.acquire();

            if (not pm_IsRunning.load(std::memory_order_acquire))
            {
                break; // Double check after wake
            }

            ProcessCommands();
        }
    }

    void
        ResourceManager::ProcessCommands()
    {
        bool f_ContainsCommands = false;

        LoadCommand f_Command;
        while (pm_LoadCommandQueue->try_dequeue(f_Command))
        {
            f_ContainsCommands = true;

            switch (f_Command.OP)
            {
            case RESOURCE_OP::LOAD_DOTNET_RUNTIME:
                break;
            case RESOURCE_OP::LOAD_TEXTURE:
            {
                LoadTexture("idfk", f_Command.NodeID);
            }
                break;
            case RESOURCE_OP::LOAD_SCENE:
            {
                //LoadScene();
            }
            break;
            default:
                PrintError("invalid opcode found for rendering manager! WHAT ARE YE DOIN SON?!?!", Colours::BrightRed);
            }
        }
    }
 
    //////////////////////////////////////////////
    // Queue Retrieval
    //////////////////////////////////////////////

    [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<LoadCommand, MOODY_CAMEL_QUEUE_SIZE>>
        ResourceManager::GetLoadCommandQueue
        (
            Logger* const logger
        ) //this is supposed to be called from the main thread so cant use the resource_logger here for thread reasons
    {
        if (not pm_IsInitialized)
        {
            logger->Error("Attempted to get a reference to ResourceManager's LoadCommandQueue before ResourceManager was initialized, please initialize ResourceManager first UwU", "ResourceManager");
            return nullptr;
        }
        else if (pm_LoadCommandQueue.use_count() >= 2)
        {
            logger->Error("Attempted to get more than one reference to ResourceManager's LoadCommandQueue >O<", "ResourceManager");
            return nullptr;
        }

        return pm_LoadCommandQueue;
    }

    //THESE METHODS ONLY ALLOW A MAXIMUM OF ONE REFERENCE PASSED OUT, TO ANYONE ASKING THIS IS MEANT FOR THE AUDIO/RENDER THREAD
    //This method should be one of the first methods called on startup

    [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>>
        ResourceManager::GetAudioResourceLoadingQueue
        (
            Logger* const logger
        ) //this is supposed to be called from the audio thread so cant use the resource_logger here for thread reasons
    {
        if (not pm_IsInitialized)
        {
            logger->Error("Attempted to get a reference to ResourceManager's AudioResourceLoadingQueue before ResourceManager was initialized, please initialize ResourceManager first UwU", "ResourceManager");
            return nullptr;
        }
        else if (pm_AudioResourceLoadingQueue.use_count() >= 2)
        {
            logger->Error("Attempted to get more than one reference to ResourceManager's AudioResourceLoadingQueue >O<", "ResourceManager");
            return nullptr;
        }

        return pm_AudioResourceLoadingQueue;
    }

    [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>>
        ResourceManager::GetDrawableResourceLoadingQueue
        (
            Logger* const logger
        ) //this is supposed to be called from the audio thread so cant use the resource_logger here for thread reasons
    {
        if (not pm_IsInitialized)
        {
            logger->Error("Attempted to get a reference to ResourceManager's DrawableResourceLoadingQueue before ResourceManager was initialized, please initialize ResourceManager first UwU", "ResourceManager");
            return nullptr;
        }
        else if (pm_DrawableResourceLoadingQueue.use_count() >= 2)
        {
            logger->Error("Attempted to get more than one reference to ResourceManager's DrawableResourceLoadingQueue >O<", "ResourceManager");
            return nullptr;
        }

        return pm_DrawableResourceLoadingQueue;
    }

    //////////////////////////////////////////////
    // Runtime Loading
    //////////////////////////////////////////////

    bool
        ResourceManager::LoadLuaRuntime()
    {

        return true;
    }

    bool
        ResourceManager::LoadDotNetRuntime
        (
            const string& fp_HostFxrPath,
            DotnetContext& fp_DotnetContext
        )
    {
        //////////////////// Load hostfxr ////////////////////

        DYNLIB_HANDLE f_HostExr = pm_DynamicLoader.LoadDynamicLibrary(fp_HostFxrPath, resource_logger.get());

        if (not f_HostExr)
        {
            resource_logger->Error(format("Failed to load hostexr at path: '{}'", fp_HostFxrPath), "ResourceManager");
            return false;
        }

        //////////////////// Store Handle in Context ////////////////////

        fp_DotnetContext.HostFxr = (hostfxr_handle)f_HostExr;

        //////////////////// Find hostfxr_initialize_for_runtime_config_fn ////////////////////

        fp_DotnetContext.RuntimeInit = (hostfxr_initialize_for_runtime_config_fn)pm_DynamicLoader.GetSymbol
        (
            "hostfxr_initialize_for_runtime_config",
            f_HostExr,
            resource_logger.get()
        );

        if (not fp_DotnetContext.RuntimeInit)
        {
            resource_logger->Error("Failed to find symbol: 'hostfxr_initialize_for_runtime_config'", "ResourceManager");
            return false;
        }

        //////////////////// Find hostfxr_get_runtime_delegate_fn ////////////////////

        fp_DotnetContext.GetDelegate = (hostfxr_get_runtime_delegate_fn)pm_DynamicLoader.GetSymbol
        (
            "hostfxr_get_runtime_delegate",
            f_HostExr,
            resource_logger.get()
        );

        if (not fp_DotnetContext.GetDelegate)
        {
            resource_logger->Error("Failed to find symbol: 'hostfxr_get_runtime_delegate'", "ResourceManager");
            return false;
        }

        //////////////////// Find load_assembly_and_get_function_pointer_fn ////////////////////

        // fp_DotnetContext.LoadAssembly = (load_assembly_and_get_function_pointer_fn)pm_DynamicLoader.GetSymbol
        // (
        //     "load_assembly_and_get_function_pointer",
        //     f_CoreClr,
        //     resource_logger.get()
        // );
        //
        // if (not fp_DotnetContext.LoadAssembly)
        // {
        //     resource_logger->LogAndPrint("Failed to find symbol: 'load_assembly_and_get_function_pointer_fn'", "ResourceManager");
        //     return false;
        // }

        //////////////////// Find hostfxr_close_fn ////////////////////

        fp_DotnetContext.Close = (hostfxr_close_fn)pm_DynamicLoader.GetSymbol
        (
            "hostfxr_close",
            f_HostExr,
            resource_logger.get()
        );

        if (not fp_DotnetContext.Close)
        {
            resource_logger->Error("Failed to find symbol: 'hostfxr_close'", "ResourceManager");
            return false;
        }

        //////////////////// Log Success and Return ////////////////////

        resource_logger->Info("Located hostfxr symbols required for running dotnet successfully!", "ResourceManager");

        return true;
    }

    //////////////////////////////////////////////
    // Main Loading Functions
    //////////////////////////////////////////////

    /*
    This should probably be used in the Peach Editor since all textures will be loaded from a compact .peachbin file that was created by the serializer
    on game export from the editor, where it will contain data flags that describe what each chunk is used for, so we can encode texture names,
    peachnode owner of the texture, where it's used, if its visible, associated shaders/pipeline, texture filtering, initial size
    */
    bool 
        ResourceManager::LoadTexture(const string& fp_TextureFilePath, const uint64_t fp_DestinationNode)
    {
        // Ensure directory exists
        if (not filesystem::exists(fp_TextureFilePath))
        {
            resource_logger->Error("Tried to pass invalid directory to LoadTextureFromFile(), Failed to load texture!", "ResourceManager");
            return false;
        }

        int f_Width, f_Height, f_Channels = 0;
        unsigned char* f_RawTextureDataPtr = nullptr;

        try
        {
            //FIX THIS NEED TO WRAP RAW PTR IN UNIQUE PTR
            f_RawTextureDataPtr = stbi_load(fp_TextureFilePath.c_str(), &f_Width, &f_Height, &f_Channels, 0);
        }
        catch (const exception& fp_Exception)
        {
            resource_logger->Error(format("Failed to load texture image!, error: '{}'", fp_Exception.what()), "ResourceManager");
            return false;
        }

        if (not f_RawTextureDataPtr)
        {
            resource_logger->Error(format("Failed to load texture! path: {}, reason: {}", fp_TextureFilePath, stbi_failure_reason()), "ResourceManager");
            return false;
        }

        unique_ptr<TextureData> f_TextureData = make_unique<TextureData>
        (
            f_RawTextureDataPtr,
            f_Width,
            f_Height,
            f_Channels
        );

        ResourceTransfer f_TextureTransfer;

        f_TextureTransfer.Payload = move(f_TextureData);
        f_TextureTransfer.NodeID = fp_DestinationNode;

        //if (not pm_DrawableResourceLoadingQueue->try_enqueue(move(f_TextureTransfer))) //if cant queue resource, put into waiting queue uwu
        //{
        //    pm_WaitingResources.push_back(move(f_TextureTransfer));
        //}

        return true; //texture loaded successfully!
    }

    bool
        ResourceManager::LoadTexture(const uint64_t* fp_TextureFilePath, const uint64_t fp_DestinationNode)
    {

        return true;
    }

    bool 
        ResourceManager::LoadWavFromFile(const string& fp_WavFilePath)
    {
    //    ifstream file(filename, ios::binary);
    //    if (!file) 
    //    {
    //        cerr << "Failed to open WAV file: " << filename << endl;
    //        return false;
    //    }

    //    char f_ChunkID[4];
    //    file.read(f_ChunkID, 4);

    //    if (strncmp(f_ChunkID, "RIFF", 4) != 0)
    //    {
    //        cerr << "Invalid WAV file: " << filename << endl;
    //        return false;
    //    }

    //    file.seekg(4, ios::cur); // Skip Chunk Size

    //    char f_Type[4];
    //    file.read(f_Type, 4);

    //    if (strncmp(f_Type, "WAVE", 4) != 0) 
    //    {
    //        cerr << "Invalid WAV file format: " << filename << endl;
    //        return false;
    //    }

    //    char f_SubChunk1ID[4];
    //    file.read(f_SubChunk1ID, 4);

    //    if (strncmp(f_SubChunk1ID, "fmt ", 4) != 0)
    //    {
    //        cerr << "Invalid WAV file fmt subchunk: " << filename << endl;
    //        return false;
    //    }

    //    uint32_t f_SubChunk1Size;
    //    file.read(reinterpret_cast<char*>(&f_SubChunk1Size), sizeof(f_SubChunk1Size));

    //    uint16_t f_AudioFormat;
    //    file.read(reinterpret_cast<char*>(&f_AudioFormat), sizeof(f_AudioFormat));

    //    uint16_t f_NumChannels;
    //    file.read(reinterpret_cast<char*>(&f_NumChannels), sizeof(f_NumChannels));

    //    uint32_t f_SampleRate;
    //    file.read(reinterpret_cast<char*>(&f_SampleRate), sizeof(f_SampleRate));

    //    file.seekg(6, ios::cur); // Skip ByteRate and BlockAlign

    //    uint16_t f_BitsPerSample;
    //    file.read(reinterpret_cast<char*>(&f_BitsPerSample), sizeof(f_BitsPerSample));

    //    char subchunk2ID[4];
    //    file.read(subchunk2ID, 4);
    //    if (strncmp(subchunk2ID, "data", 4) != 0) {
    //        cerr << "Invalid WAV file data subchunk: " << filename << endl;
    //        return false;
    //    }

    //    uint32_t subchunk2Size;
    //    file.read(reinterpret_cast<char*>(&subchunk2Size), sizeof(subchunk2Size));

    //    vector<char> data(subchunk2Size);
    //    file.read(data.data(), subchunk2Size);

    //    ALenum format;
    //    if (f_NumChannels == 1) {
    //        format = (f_BitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    //    }
    //    else {
    //        format = (f_BitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    //    }

    //    alBufferData(buffer, format, data.data(), subchunk2Size, f_SampleRate);

        return true;
    }

    bool
        ResourceManager::LoadCompiledShader(const string& fp_ShaderFilePath)
    {

        return true;
    }

    /* 
    Called by the rendering thread  //TODO TRY PUSHING THE PACKAGES IN VECTORS ALWAYS AND ONLY UNPACK IN RENDERINGMANAGER,
    WE SHOULD FIGURE OUT A WAY TO LOAD THINGS IN BUNCHES, OR GET ALL LOADED OBJECTS IN ONEGO AND ONLY PUSH THEM ONCE ITS ALL LOADED
    THAT WOULD BE GOOD FOR LOADING SCENES, WE DONT WANT TO PUSH ANY RESOURCES EARLIER THAN NEEDED UNTIL THE ENTIRE SCENE IS LOADED
    I'm kinda tired of working on the loading manager and i wanna do physics now so gl future ryan i hope things go well >w< 
    */
    void 
        ResourceManager::TryPushingResourceTransfer
        (
            ResourceTransfer&& fp_ResourceTransfer
        )
    {
        //pm_WaitingLoadedGraphicsAssets.emplace_back(fp_ObjectID, move(fp_TextureDataPtr)); //construct package in vector

        //if (not pm_DrawableResourceLoadingQueue->PushLoadedResourcePackages(pm_WaitingLoadedGraphicsAssets))
        //{
        //    resource_logger->Trace("Load put off until later", "ResourceManager");
        //    return false;
        //}

        //return true;
    }

    bool
        ResourceManager::LoadPlugin
        (
            const string& fp_PluginFilePath,
            PluginData& fp_Plugin
        )
        const
    {
        DYNLIB_HANDLE f_Handle;

        if (not (filesystem::exists(fp_PluginFilePath) and filesystem::is_regular_file(fp_PluginFilePath)))
        {
            resource_logger->Error("Failed to locate DLL at: " + fp_PluginFilePath, "ResourceManager");
            return false;
        }

        f_Handle = DYNLIB_LOAD(fp_PluginFilePath.c_str());
        resource_logger->Debug("Successfully located DLL at: " + fp_PluginFilePath, "ResourceManager");

        if (not f_Handle)
        {
            resource_logger->Error("Failed to load plugin at path: " + fp_PluginFilePath, "ResourceManager");
            return false;
        }

        resource_logger->Debug("Successfully loaded plugin at: " + fp_PluginFilePath, "ResourceManager");

        auto f_CreateFunc = (CreatePluginFunc)DYNLIB_GETSYM(f_Handle, "createPlugin");
        auto f_DestroyFunc = (DestroyPluginFunc)DYNLIB_GETSYM(f_Handle, "destroyPlugin");

        if (not f_CreateFunc or not f_DestroyFunc)
        {
            resource_logger->Error("Failed to find CreatePlugin() or DestroyPlugin() functions in: " + fp_PluginFilePath, "ResourceManager");
            DYNLIB_UNLOAD(f_Handle);
            return false;
        }

        resource_logger->Debug("Successfully located CreatePlugin() or DestroyPlugin() functions in: " + fp_PluginFilePath, "ResourceManager");

        fp_Plugin.Pwugin = unique_ptr<Plugin, DestroyPluginFunc>(f_CreateFunc(), f_DestroyFunc); //creates smrt poiner with destructor tied to it;
        fp_Plugin.Handle = f_Handle;

        return true;
    }

    ////////////////////////////////////////////////
    // Directory Detection Functions
    ////////////////////////////////////////////////

    void
        ResourceManager::CheckForDirectoryChanges()
    {
        static unordered_map<string, PHYSFS_sint64> lastModifiedTimes;

        char** rc = PHYSFS_enumerateFiles("/");

        for (char** i = rc; *i != NULL; i++)
        {
            string fullPath = string("/") + *i;
            PHYSFS_Stat stat;

            if (PHYSFS_stat(fullPath.c_str(), &stat))
            {
                if (lastModifiedTimes.find(fullPath) == lastModifiedTimes.end() or lastModifiedTimes[fullPath] != stat.modtime)
                {
                    // File has changed or is new
                    //processFileChange(fullPath);
                    // Update the last modified time
                    lastModifiedTimes[fullPath] = stat.modtime;
                }
            }
        }

        PHYSFS_freeList(rc);
    }

    // Function to list all files recursively
    unordered_map<string, filesystem::file_time_type>
        ResourceManager::GetCurrentDirectoryState
        (
            const filesystem::path& fp_Directory
        )
    {
        unordered_map<string, filesystem::file_time_type> f_Files;

        try
        {
            for (const auto& _entry : filesystem::recursive_directory_iterator(fp_Directory))
            {
                if (filesystem::is_regular_file(_entry.status()) or filesystem::is_directory(_entry.status()))
                {
                    f_Files[_entry.path().string()] = filesystem::last_write_time(_entry);
                }
            }
        }
        catch (const filesystem::filesystem_error& e)
        {
            resource_logger->Error("LogAndPrint while checking current directory state: " + static_cast<string>(e.what()), "main");
        }

        return f_Files;
    }

    bool
        ResourceManager::CompareStates
        (
            const unordered_map<string, filesystem::file_time_type>& fp_OldState,
            const unordered_map <string, filesystem::file_time_type>& fp_NewState
        )
    {
        for (const auto& _file : fp_NewState)
        {
            auto it = fp_OldState.find(_file.first);

            if (it == fp_OldState.end())
            {
                resource_logger->Debug("New file found in working directory: " + _file.first, "main");
                return false;
            }
            else if (it->second != _file.second)
            {
                resource_logger->Trace("Modified file found in working directory: " + _file.first, "main");
                return false;
            }
        }

        for (const auto& _file : fp_OldState)
        {
            if (fp_NewState.find(_file.first) == fp_NewState.end())
            {
                resource_logger->Debug("Deleted file from working directory: " + _file.first, "main");
                return false;
            }
        }

        return true;
    }

    void
        ResourceManager::CheckAndUpdateFileSystem() //XXX: this function seems kinda sus idk if it works as i want it too lmfao
    {
        auto f_CurrentPath = filesystem::current_path(); //idfk
        auto f_InitialPathState = GetCurrentDirectoryState(f_CurrentPath);

        auto f_NewState = GetCurrentDirectoryState(f_CurrentPath);

        if (f_NewState != f_InitialPathState)
        {
            CompareStates(f_InitialPathState, f_NewState);
            f_InitialPathState = move(f_NewState);
        }
    }
}