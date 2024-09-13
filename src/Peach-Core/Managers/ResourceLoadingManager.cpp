#include "../../include/Peach-Core/Managers/ResourceLoadingManager.h"

namespace PeachCore {
	ResourceLoadingManager::~ResourceLoadingManager()
	{

	}

	ResourceLoadingManager::ResourceLoadingManager()
	{

	}

	//THESE METHODS ONLY ALLOW A MAXIMUM OF TWO REFERENCES PASSED OUT TO ANYONE ASKING THIS IS MEANT FOR THE MAIN THREAD AND AUDIO/RENDER THREAD

	shared_ptr<LoadingQueue> ResourceLoadingManager::GetAudioResourceLoadingQueue() //This method should be one of the first methods called on startup
	{
		assert(pm_AudioQueueReferenceCount <= 2);

		if (pm_AudioQueueReferenceCount == 2)  //stops unwanted extra references from being created accidentally
		{
			LogManager::ResourceLoadingLogger().Warn("Attempted to get more than 2 references to PeachEngineResourceLoadingManager's AudioResourceLoadingQueue", "PeachEngineResourceLoadingManager");
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
		ResourceLoadingManager::GetDrawableResourceLoadingQueue() //This method should be one of the first methods called on startup
	{
		assert(pm_DrawableQueueReferenceCount <= 2);

		if (pm_DrawableQueueReferenceCount == 2) //stops unwanted extra references from being created accidentally
		{
			LogManager::ResourceLoadingLogger().Warn("Attempted to get more than 2 references to PeachEngineResourceLoadingManager's DrawableResourceLoadingQueue", "PeachEngineResourceLoadingManager");
			return nullptr;
		}
		else if (pm_DrawableQueueReferenceCount == 0) //lazy initialization for LoadingQueue cause why not
		{
			pm_DrawableResourceLoadingQueue = make_shared<PeachCore::LoadingQueue>();
		}

		pm_DrawableQueueReferenceCount++;
		return pm_DrawableResourceLoadingQueue;
	}



	//bool ResourceLoadingManager::LoadTextureFromSpecifiedFilePath(const string& fp_TextureFilePath)
	//{
	//	// Load image data (consider using stb_image or any similar library)
	//	int width, height, nrChannels;
	//	unique_ptr<unsigned char> f_TextureData = nullptr;
	//	try
	//	{
	//		f_TextureData = make_unique<unsigned char>(stbi_load(fp_TextureFilePath.c_str(), &width, &height, &nrChannels, 0));
	//	}
	//	catch (const exception& ex)
	//	{
	//		LogManager::ResourceLoadingLogger().Error( "Failed to load texture image!", "ResourceLoadingManager");
	//		return false;
	//	}

	//	if (f_TextureData)
	//	{
	//		TryPushingLoadedResourcePackage(move(make_unique<LoadedResourcePackage>("someObjectID", move(f_TextureData))));
	//		stbi_image_free(f_TextureData.get()); //can we unload right away?
	//		return true;
	//	}

	//	return false; // Handle failed load appropriately
	//}

		// Called by the rendering thread  //TODO TRY PUSHING THE PACKAGES IN VECTORS ALWAYS AND ONLY UNPACK IN RENDERINGMANAGER,
		//WE SHOULD FIGURE OUT A WAY TO LOAD THINGS IN BUNCHES, OR GET ALL LOADED OBJECTS IN ONEGO AND ONLY PUSH THEM ONCE ITS ALL LOADED
		//THAT WOULD BE GOOD FOR LOADING SCENES, WE DONT WANT TO PUSH ANY RESOURCES EARLIER THAN NEEDED UNTIL THE ENTIRE SCENE IS LOADED
		//I'm kinda tired of working on the loading manager and i wanna do physics now so gl future ryan i hope things go well >w<
	bool ResourceLoadingManager::TryPushingLoadedResourcePackage(unique_ptr<LoadedResourcePackage> fp_LoadedPackage)
	{
		pm_WaitingFullyLoadedResourcePackages.push_back(move(fp_LoadedPackage));

		if (!pm_DrawableResourceLoadingQueue->PushLoadedResourcePackage(pm_WaitingFullyLoadedResourcePackages))
		{
			LogManager::ResourceLoadingLogger().Debug("Load put off until later", "ResourceLoadingManager");
			return false;
		}

		return true;
	}
}