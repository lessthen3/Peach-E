#include "../../include/Peach-Editor/Managers/PeachEditorResourceLoadingManager.h"

namespace PeachEditor {

	PeachEditorResourceLoadingManager::~PeachEditorResourceLoadingManager()
	{

	}

	PeachEditorResourceLoadingManager::PeachEditorResourceLoadingManager()
	{

	}

	//THESE METHODS ONLY ALLOW A MAXIMUM OF TWO REFERENCES PASSED OUT TO ANYONE ASKING THIS IS MEANT FOR THE MAIN THREAD AND AUDIO/RENDER THREAD

	shared_ptr<PeachCore::LoadingQueue> PeachEditorResourceLoadingManager::GetAudioResourceLoadingQueue() //This method should be one of the first methods called on startup
	{
		lock_guard<mutex> lock(PeachEditorResourceMutex);

		assert(pm_AudioQueueReferenceCount <= 2);

		if (pm_AudioQueueReferenceCount == 2)  //stops unwanted extra references from being created accidentally
		{
			InternalLogManager::InternalResourceLoadingLogger().Warn("Attempted to get more than 2 references to PeachEditorResourceLoadingManager's AudioResourceLoadingQueue", "PeachEditorResourceLoadingManager");
			return nullptr;
		}
		else if (pm_AudioResourceLoadingQueue == 0) //lazy initialization for LoadingQueue cause why not
		{
			pm_AudioResourceLoadingQueue = make_shared<PeachCore::LoadingQueue>();
		}

		pm_AudioQueueReferenceCount++;
		return pm_AudioResourceLoadingQueue;
	}

	shared_ptr<PeachCore::LoadingQueue> PeachEditorResourceLoadingManager::GetDrawableResourceLoadingQueue() //This method should be one of the first methods called on startup
	{
		lock_guard<mutex> lock(PeachEditorResourceMutex);

		assert(pm_DrawableQueueReferenceCount <= 2);

		if (pm_DrawableQueueReferenceCount == 2) //stops unwanted extra references from being created accidentally
		{
			InternalLogManager::InternalResourceLoadingLogger().Warn("Attempted to get more than 2 references to PeachEditorResourceLoadingManager's DrawableResourceLoadingQueue", "PeachEditorResourceLoadingManager");
			return nullptr;
		}
		else if (pm_DrawableQueueReferenceCount == 0) //lazy initialization for LoadingQueue cause why not
		{
			pm_DrawableResourceLoadingQueue = make_shared<PeachCore::LoadingQueue>();
		}

		pm_DrawableQueueReferenceCount++;
		return pm_DrawableResourceLoadingQueue;
	}



	//bool PeachEditorResourceLoadingManager::LoadTextureFromSpecifiedFilePath(const string& fp_TextureFilePath)
	//{
	//	lock_guard<mutex> lock(PeachEditorResourceMutex);

	//	// Load image data (consider using stb_image or any similar library)
	//	int width, height, nrChannels;
	//	
	//	unique_ptr<unsigned char> f_TextureData(stbi_load(fp_TextureFilePath.c_str(), &width, &height, &nrChannels, 0));

	//	if (f_TextureData)
	//	{
	//		TryPushingLoadedResourcePackage(move(make_unique<PeachCore::LoadedResourcePackage>("someObjectID", move(f_TextureData))));
	//		return true;
	//	}
	//	
	//	else
	//	{
	//		InternalLogManager::InternalResourceLoadingLogger().Error("Failed to load texture image!", "ResourceLoadingManager");
	//		return false;
	//	}
	//	

	//	return false; // Handle failed load appropriately
	//}

	// Called by the rendering thread  //TODO TRY PUSHING THE PACKAGES IN VECTORS ALWAYS AND ONLY UNPACK IN RENDERINGMANAGER,
	//WE SHOULD FIGURE OUT A WAY TO LOAD THINGS IN BUNCHES, OR GET ALL LOADED OBJECTS IN ONEGO AND ONLY PUSH THEM ONCE ITS ALL LOADED
	//THAT WOULD BE GOOD FOR LOADING SCENES, WE DONT WANT TO PUSH ANY RESOURCES EARLIER THAN NEEDED UNTIL THE ENTIRE SCENE IS LOADED
	//I'm kinda tired of working on the loading manager and i wanna do physics now so gl future ryan i hope things go well >w<
	bool PeachEditorResourceLoadingManager::TryPushingLoadedResourcePackage(unique_ptr<PeachCore::LoadedResourcePackage> fp_LoadedPackage)
	{
		pm_WaitingFullyLoadedResourcePackages.push_back(move(fp_LoadedPackage));

		if (!pm_DrawableResourceLoadingQueue->PushLoadedResourcePackage(pm_WaitingFullyLoadedResourcePackages))
		{
			InternalLogManager::InternalResourceLoadingLogger().Debug("Load deferred until later", "PeachEditorResourceLoadingManager");
			return false;
		}

		return true;
	}
}