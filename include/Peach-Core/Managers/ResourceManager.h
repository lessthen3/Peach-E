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
#include "../General/Plugin.h"

#include "../Utils/ShaderUtils.h"
#include "../Utils/Serializer.h"
#include "../Utils/LoadingQueue.h"
#include "../Utils/CommandQueue.h"
#include "../Utils/DynamicLoader.h"

///External
#include <physfs.h>
#include <stb/stb_image.h>
#include <miniaudio/miniaudio.h>

typedef Plugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(Plugin*);

namespace PeachCore {

	struct PluginInfo
	{
		unique_ptr<Plugin, DestroyPluginFunc> Pwugin = { nullptr, nullptr }; //>O<
		DYNLIB_HANDLE Handle = nullptr; //>w<
	};

	class ResourceManager 
	{
	//////////////////////////////////////////////
	// Private Destructor
	//////////////////////////////////////////////
	private:
		~ResourceManager() = default;

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
		//used to push loaded assets that are destined for AudioManager
		shared_ptr<LoadingQueue> pm_AudioResourceLoadingQueue = nullptr;
		//used to push loaded assets that are destined for RenderingManager
		shared_ptr<LoadingQueue> pm_DrawableResourceLoadingQueue = nullptr; 
		//used for asking ResourceManager to load something from the main thread
		shared_ptr<CommandQueue> pm_LoadCommandQueue = nullptr;

		bool pm_IsInitialized = false;

		// Holds mesh, texture, shader and animation data
		vector<LoadedResourcePackage> pm_WaitingLoadedGraphicsAssets;
		//Holds mp3, wav and flac files
		vector<LoadedResourcePackage> pm_WaitingLoadedAudioAssets;

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

		[[nodiscard]] shared_ptr<LoadingQueue>
			GetAudioResourceLoadingQueue();

		[[nodiscard]] shared_ptr<LoadingQueue>
			GetDrawableResourceLoadingQueue();

		bool
			LoadPlugin
			(
				const string& fp_PluginFilePath,
				PluginInfo& fp_Plugin
			);

		bool 
			LoadTextureFromFile(const string& fp_TextureFilePath);

		bool
			LoadWavFromFile(const string& fp_WavFilePath);

	//////////////////////////////////////////////
	// Private Methods
	//////////////////////////////////////////////
	private:
		bool 
			TryPushingLoadedTexture
			(
				const string& fp_ObjectID, 
				unique_ptr<TextureData> fp_TextureDataPtr
			);
	};
}