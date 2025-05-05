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
		resource_logger = make_unique<LogManager>();
		resource_logger->Initialize(ThreadName::ResourceThread, fp_LogOutputDirectory, "ResourceManager", fp_Console, LogManager::LogLevel::All);
		resource_logger->LogAndPrint("ResourceLoadingLogger successfully initialized", "ResourceLoadingManager", LogManager::LogLevel::Debug);

		return true;
	}

	//THESE METHODS ONLY ALLOW A MAXIMUM OF TWO REFERENCES PASSED OUT TO ANYONE ASKING THIS IS MEANT FOR THE MAIN THREAD AND AUDIO/RENDER THREAD

	shared_ptr<LoadingQueue> 
		ResourceManager::GetAudioResourceLoadingQueue
		(
		) //This method should be one of the first methods called on startup
	{
		assert(pm_AudioQueueReferenceCount <= 2);

		if (pm_AudioQueueReferenceCount == 2)  //stops unwanted extra references from being created accidentally
		{
			resource_logger->LogAndPrint("Attempted to get more than 2 references to ResourceManager's AudioResourceLoadingQueue", "ResourceManager", LogManager::LogLevel::Warning);
			return nullptr;
		}
		else if (pm_AudioResourceLoadingQueue == 0) //lazy initialization for LoadingQueue cause why not
		{
			pm_AudioResourceLoadingQueue = make_shared<PeachCore::LoadingQueue>();
		}

		pm_AudioQueueReferenceCount++;
		return pm_AudioResourceLoadingQueue;
	}

	shared_ptr<LoadingQueue> 
		ResourceManager::GetDrawableResourceLoadingQueue
		(
		) //This method should be one of the first methods called on startup
	{
		assert(pm_DrawableQueueReferenceCount <= 2);

		if (pm_DrawableQueueReferenceCount == 2) //stops unwanted extra references from being created accidentally
		{
			resource_logger->LogAndPrint("Attempted to get more than 2 references to ResourceManager's DrawableResourceLoadingQueue", "ResourceManager", LogManager::LogLevel::Warning);
			return nullptr;
		}
		else if (pm_DrawableQueueReferenceCount == 0) //lazy initialization for LoadingQueue cause why not
		{
			pm_DrawableResourceLoadingQueue = make_shared<PeachCore::LoadingQueue>();
		}

		pm_DrawableQueueReferenceCount++;
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
		pm_WaitingFullyLoadedResourcePackages.emplace_back(fp_ObjectID, move(fp_TextureDataPtr)); //construct package in vector

		if (not pm_DrawableResourceLoadingQueue->PushLoadedResourcePackages(pm_WaitingFullyLoadedResourcePackages))
		{
			resource_logger->LogAndPrint("Load put off until later", "ResourceManager", LogManager::LogLevel::Trace);
			return false;
		}

		return true;
	}
}