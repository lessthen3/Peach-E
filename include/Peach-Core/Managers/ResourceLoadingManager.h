#pragma once

#include "LogManager.h"

#include "../General/LoadingQueue.h"

namespace PeachCore {

	class ResourceLoadingManager {
	public:
		static ResourceLoadingManager& ResourceLoader() {
			static ResourceLoadingManager resourceloader;
			return resourceloader;
		}

		~ResourceLoadingManager();

	public:
		mutex resourceMutex;

	private:
		ResourceLoadingManager();

		ResourceLoadingManager(const ResourceLoadingManager&) = delete;
		ResourceLoadingManager& operator=(const ResourceLoadingManager&) = delete;




	public:
		shared_ptr<LoadingQueue> GetAudioResourceLoadingQueue();
		shared_ptr<LoadingQueue> GetDrawableResourceLoadingQueue();

	public: //PUBLIC FOR TESTING
		bool TryPushingLoadedResourcePackage(unique_ptr<LoadedResourcePackage> fp_LoadedPackage);
		bool LoadTextureFromSpecifiedFilePath(const string& fp_FilePath);

	private:
		shared_ptr<LoadingQueue> pm_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager
		shared_ptr<LoadingQueue> pm_DrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

		unsigned int pm_AudioQueueReferenceCount = 0;
		unsigned int pm_DrawableQueueReferenceCount = 0;

		vector<unique_ptr<LoadedResourcePackage>> pm_WaitingFullyLoadedResourcePackages;
	};

}