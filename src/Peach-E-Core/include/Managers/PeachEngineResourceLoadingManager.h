#pragma once

#include "InternalLogManager.h"

#include "../../../Peach-core/include/Unsorted/CommandQueue.h"
#include "../../../Peach-core/include/Unsorted/LoadingQueue.h"

namespace PeachEngine {

	class PeachEngineResourceLoadingManager {
	public:
		static PeachEngineResourceLoadingManager& PeachEngineResourceLoader() {
			static PeachEngineResourceLoadingManager resourceloader;
			return resourceloader;
		}

		~PeachEngineResourceLoadingManager();

	public:
		mutex resourceMutex;

	private:
		PeachEngineResourceLoadingManager();

		PeachEngineResourceLoadingManager(const PeachEngineResourceLoadingManager&) = delete;
		PeachEngineResourceLoadingManager& operator=(const PeachEngineResourceLoadingManager&) = delete;




	public:
		shared_ptr<PeachCore::LoadingQueue> GetAudioResourceLoadingQueue();
		shared_ptr<PeachCore::LoadingQueue> GetDrawableResourceLoadingQueue();

	public: //PUBLIC FOR TESTING
		bool TryPushingLoadedResourcePackage(unique_ptr<PeachCore::LoadedResourcePackage> fp_LoadedPackage);
		bool LoadTextureFromSpecifiedFilePath(const string& fp_FilePath);

	private:
		shared_ptr<PeachCore::LoadingQueue> pm_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager
		shared_ptr<PeachCore::LoadingQueue> pm_DrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

		unsigned int pm_AudioQueueReferenceCount = 0;
		unsigned int pm_DrawableQueueReferenceCount = 0;

		vector<unique_ptr<PeachCore::LoadedResourcePackage>> pm_WaitingFullyLoadedResourcePackages;
	};

}
