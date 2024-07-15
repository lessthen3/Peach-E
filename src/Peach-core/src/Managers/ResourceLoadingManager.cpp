#include "../../include/Managers/ResourceLoadingManager.h"

namespace PeachCore {
	ResourceLoadingManager::~ResourceLoadingManager()
	{

	}

	ResourceLoadingManager::ResourceLoadingManager()
	{

	}

	shared_ptr<LoadingQueue> ResourceLoadingManager::GetAudioResourceLoadingQueue() //This method should be one of the first methods called on startup
	{
		if(pm_AudioQueueReferenceCount == 2)  //stops unwanted extra references from being created accidentally
			{return nullptr;}
		else if(pm_AudioQueueReferenceCount == 0)  //lazy initialization for LoadingQueue cause why not
			{pm_AudioResourceLoadingQueue = make_shared<LoadingQueue>();}

		pm_AudioQueueReferenceCount++;
		return pm_AudioResourceLoadingQueue;
	}

	shared_ptr<LoadingQueue> ResourceLoadingManager::GetDrawableResourceLoadingQueue() //This method should be one of the first methods called on startup
	{
		if (pm_DrawableQueueReferenceCount == 2) //stops unwanted extra references from being created accidentally
			{return nullptr;}
		else if(pm_DrawableQueueReferenceCount == 0) //lazy initialization for LoadingQueue cause why not
			{pm_DrawableResourceLoadingQueue = make_shared<LoadingQueue>();}

		pm_DrawableQueueReferenceCount++;
		return pm_DrawableResourceLoadingQueue;
	}



	bool ResourceLoadingManager::LoadTexture(const string& fp_FilePath)
	{
		auto texture = make_unique<sf::Texture>();

		if (texture->loadFromFile(fp_FilePath))
		{
			TryPushingLoadedResourcePackage( move(make_unique<LoadedResourcePackage>("someObjectID", move(texture))) );
			return true;
		}

		return false; // Handle failed load appropriately
	}

		// Called by the rendering thread
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