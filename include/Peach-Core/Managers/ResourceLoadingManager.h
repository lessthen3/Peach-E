/*******************************************************************
 *                                             Peach-E v0.1
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#pragma once

///STL
#include <memory>
#include <assert.h>

///Peach-E
#include "../General/Serializer.h"
#include "../General/LoadingQueue.h"

///External
#include <physfs.h>

namespace PeachCore {

	class ResourceLoadingManager 
	{
	//////////////////////////////////////////////
	// Private Destructor
	//////////////////////////////////////////////
	private:
		~ResourceLoadingManager();

	//////////////////////////////////////////////
	// Singleton Instance
	//////////////////////////////////////////////
	public:
		static ResourceLoadingManager& ResourceLoader() 
		{
			static ResourceLoadingManager resourceloader;
			return resourceloader;
		}

	//////////////////////////////////////////////
	// Private Constructor
	//////////////////////////////////////////////
	private:
		ResourceLoadingManager();

		ResourceLoadingManager(const ResourceLoadingManager&) = delete;
		ResourceLoadingManager& operator=(const ResourceLoadingManager&) = delete;

	//////////////////////////////////////////////
	// Private Members
	//////////////////////////////////////////////
	private:
		shared_ptr<LoadingQueue> pm_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager
		shared_ptr<LoadingQueue> pm_DrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

		unsigned int pm_AudioQueueReferenceCount = 0;
		unsigned int pm_DrawableQueueReferenceCount = 0;

		vector<unique_ptr<LoadedResourcePackage>> pm_WaitingFullyLoadedResourcePackages;

		unique_ptr<LogManager> resource_logger = nullptr;

	//////////////////////////////////////////////
	// Public Members
	//////////////////////////////////////////////
	public:
		mutex resourceMutex;

	//////////////////////////////////////////////
	// Public Methods
	//////////////////////////////////////////////
	public:
		bool 
			Initialize
		(
			const string& fp_LogOutputDirectory,
			shared_ptr<Console> fp_Console
		);

		shared_ptr<LoadingQueue> GetAudioResourceLoadingQueue();
		shared_ptr<LoadingQueue> GetDrawableResourceLoadingQueue();

	public: //PUBLIC FOR TESTING
		bool TryPushingLoadedResourcePackage(unique_ptr<LoadedResourcePackage> fp_LoadedPackage);
		bool LoadTextureFromFile(const string& fp_FilePath);

	//////////////////////////////////////////////
	// Private Methods
	//////////////////////////////////////////////
	private:


	};
}