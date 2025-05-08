/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#include "../../include/Peach-Core/Managers/ResourceManager.h"

namespace PeachCore {

	bool
		ResourceManager::Initialize
		(
			const string& fp_LogOutputDirectory,
			shared_ptr<Console> fp_Console
		)
	{
		if (not fp_Console)
		{
			PrintError("Tried to initialize ResourceManager with a nullptr reference to the Console");
			return false;
		}

		resource_logger = make_unique<LogManager>();
		resource_logger->Initialize(ThreadName::ResourceThread, fp_LogOutputDirectory, "ResourceThreadLogger", fp_Console, LogManager::LogLevel::All);
		resource_logger->LogAndPrint("ResourceLoadingLogger successfully initialized", "ResourceLoadingManager", LogManager::LogLevel::Debug);

		pm_AudioResourceLoadingQueue = make_shared<LoadingQueue>();
		pm_DrawableResourceLoadingQueue = make_shared<LoadingQueue>();

		pm_IsInitialized = true;

		return true;
	}

	//THESE METHODS ONLY ALLOW A MAXIMUM OF ONE REFERENCE PASSED OUT, TO ANYONE ASKING THIS IS MEANT FOR THE AUDIO/RENDER THREAD

	[[nodiscard]] shared_ptr<LoadingQueue>
		ResourceManager::GetAudioResourceLoadingQueue() //This method should be one of the first methods called on startup
	{
		if (not pm_IsInitialized)
		{
			resource_logger->LogAndPrint("Attempted to get a reference to ResourceManager's AudioResourceLoadingQueue before ResourceManager was initialized, please initialize ResourceManager first UwU", "ResourceManager", LogManager::LogLevel::Error);
			return nullptr;
		}
		else if (pm_AudioResourceLoadingQueue.use_count() >= 2)
		{
			resource_logger->LogAndPrint("Attempted to get more than one reference to ResourceManager's AudioResourceLoadingQueue >O<", "ResourceManager", LogManager::LogLevel::Error);
			return nullptr;
		}

		return pm_AudioResourceLoadingQueue;
	}

	[[nodiscard]] shared_ptr<LoadingQueue>
		ResourceManager::GetDrawableResourceLoadingQueue() //This method should be one of the first methods called on startup
	{
		if (not pm_IsInitialized)
		{
			resource_logger->LogAndPrint("Attempted to get a reference to ResourceManager's DrawableResourceLoadingQueue before ResourceManager was initialized, please initialize ResourceManager first UwU", "ResourceManager", LogManager::LogLevel::Error);
			return nullptr;
		}
		else if (pm_DrawableResourceLoadingQueue.use_count() >= 2)
		{
			resource_logger->LogAndPrint("Attempted to get more than one reference to ResourceManager's DrawableResourceLoadingQueue >O<", "ResourceManager", LogManager::LogLevel::Error);
			return nullptr;
		}

		return pm_DrawableResourceLoadingQueue;
	}


	/*
	This should probably be used in the Peach Editor since all textures will be loaded from a compact .peachbin file that was created by the serializer
	on game export from the editor, where it will contain data flags that describe what each chunk is used for, so we can encode texture names,
	peachnode owner of the texture, where it's used, if its visible, associated shaders/pipeline, texture filtering, initial size
	*/
	bool 
		ResourceManager::LoadTextureFromFile(const string& fp_TextureFilePath)
	{
		// Ensure directory exists
		if (not filesystem::exists(fp_TextureFilePath))
		{
			resource_logger->LogAndPrint("Tried to pass invalid directory to LoadTextureFromFile(), Failed to load texture!", "ResourceManager", LogManager::LogLevel::Error);
			return false;
		}

		int width, height, nrChannels = 0;
		unsigned char* f_RawTextureDataPtr = nullptr;

		try
		{
			//FIX THIS NEED TO WRAP RAW PTR IN UNIQUE PTR
			f_RawTextureDataPtr = stbi_load(fp_TextureFilePath.c_str(), &width, &height, &nrChannels, 0);
		}
		catch (const exception& ex)
		{
			resource_logger->LogAndPrint(format("Failed to load texture image!, error: '{}'", ex.what()), "ResourceManager", LogManager::LogLevel::Error);
			return false;
		}

		if (not f_RawTextureDataPtr)
		{
			resource_logger->LogAndPrint("Failed to load texture image!", "ResourceManager", LogManager::LogLevel::Error);
			return false;
		}

		unique_ptr<TextureData> f_TextureData = make_unique<TextureData>
		(
			f_RawTextureDataPtr,
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height),
			static_cast<uint32_t>(nrChannels)
		);

		TryPushingLoadedTexture("someObjectID", move(f_TextureData));

		return true; //texture loaded successfully!
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

	/* 
	Called by the rendering thread  //TODO TRY PUSHING THE PACKAGES IN VECTORS ALWAYS AND ONLY UNPACK IN RENDERINGMANAGER,
	WE SHOULD FIGURE OUT A WAY TO LOAD THINGS IN BUNCHES, OR GET ALL LOADED OBJECTS IN ONEGO AND ONLY PUSH THEM ONCE ITS ALL LOADED
	THAT WOULD BE GOOD FOR LOADING SCENES, WE DONT WANT TO PUSH ANY RESOURCES EARLIER THAN NEEDED UNTIL THE ENTIRE SCENE IS LOADED
	I'm kinda tired of working on the loading manager and i wanna do physics now so gl future ryan i hope things go well >w< 
	*/
	bool 
		ResourceManager::TryPushingLoadedTexture
		(
			const string& fp_ObjectID,
			unique_ptr<TextureData> fp_TextureDataPtr
		)
	{
		pm_WaitingLoadedGraphicsAssets.emplace_back(fp_ObjectID, move(fp_TextureDataPtr)); //construct package in vector

		if (not pm_DrawableResourceLoadingQueue->PushLoadedResourcePackages(pm_WaitingLoadedGraphicsAssets))
		{
			resource_logger->LogAndPrint("Load put off until later", "ResourceManager", LogManager::LogLevel::Trace);
			return false;
		}

		return true;
	}

	bool
		ResourceManager::LoadPlugin
		(
			const string& fp_PluginFilePath,
			PluginInfo& fp_Plugin
		)
	{
		DYNLIB_HANDLE f_Handle;

		if (not (filesystem::exists(fp_PluginFilePath) and filesystem::is_regular_file(fp_PluginFilePath)))
		{
			resource_logger->LogAndPrint("Failed to locate DLL at: " + fp_PluginFilePath, "ResourceManager", LogManager::LogLevel::Error);
			return false;
		}

		f_Handle = DYNLIB_LOAD(fp_PluginFilePath.c_str());
		resource_logger->LogAndPrint("Successfully located DLL at: " + fp_PluginFilePath, "ResourceManager", LogManager::LogLevel::Debug);

		if (not f_Handle)
		{
			resource_logger->LogAndPrint("Failed to load plugin at path: " + fp_PluginFilePath, "ResourceManager", LogManager::LogLevel::Error);
			return false;
		}

		resource_logger->LogAndPrint("Successfully loaded plugin at: " + fp_PluginFilePath, "ResourceManager", LogManager::LogLevel::Debug);

		auto f_CreateFunc = (CreatePluginFunc)DYNLIB_GETSYM(f_Handle, "createPlugin");
		auto f_DestroyFunc = (DestroyPluginFunc)DYNLIB_GETSYM(f_Handle, "destroyPlugin");

		if (not f_CreateFunc or not f_DestroyFunc)
		{
			resource_logger->LogAndPrint("Failed to find CreatePlugin() or DestroyPlugin() functions in: " + fp_PluginFilePath, "ResourceManager", LogManager::LogLevel::Error);
			DYNLIB_UNLOAD(f_Handle);
			return false;
		}

		resource_logger->LogAndPrint("Successfully located CreatePlugin() or DestroyPlugin() functions in: " + fp_PluginFilePath, "ResourceManager", LogManager::LogLevel::Debug);

		fp_Plugin.Pwugin = unique_ptr<Plugin, DestroyPluginFunc>(f_CreateFunc(), f_DestroyFunc); //creates smrt poiner with destructor tied to it;
		fp_Plugin.Handle = f_Handle;

		return true;
	}
}