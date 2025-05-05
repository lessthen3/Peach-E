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
#pragma once

///PeachCore
#include "../Rendering/ShaderUtils.h"
#include "../General/Serializer.h"
#include "../General/LoadingQueue.h"

///STL
#include <memory>
#include <assert.h>

///External
#include <physfs.h>
#include <stb/stb_image.h>

namespace PeachCore {

	class ResourceManager 
	{
	//////////////////////////////////////////////
	// Private Destructor
	//////////////////////////////////////////////
	private:
		~ResourceManager() {};

	//////////////////////////////////////////////
	// Singleton Instance
	//////////////////////////////////////////////
	public:
		static ResourceManager& ResourceLoader()
		{
			static ResourceManager resourceloader;
			return resourceloader;
		}

	//////////////////////////////////////////////
	// Private Constructor
	//////////////////////////////////////////////
	private:
		ResourceManager() = default;

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;

	//////////////////////////////////////////////
	// Private Members
	//////////////////////////////////////////////
	private:
		shared_ptr<LoadingQueue> pm_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager
		shared_ptr<LoadingQueue> pm_DrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

		unsigned int pm_AudioQueueReferenceCount = 0;
		unsigned int pm_DrawableQueueReferenceCount = 0;

		vector<LoadedResourcePackage> pm_WaitingFullyLoadedResourcePackages;

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
		bool TryPushingLoadedTexture(const string& fp_ObjectID, unique_ptr<TextureData> fp_TextureDataPtr);
		bool LoadTextureFromFile(const string& fp_FilePath);

	//////////////////////////////////////////////
	// Private Methods
	//////////////////////////////////////////////
	private:


	};
}