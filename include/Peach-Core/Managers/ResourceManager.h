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
#include "../General/Plugin.h"

#include "../General/DynamicLoader.h"

///STL
#include <memory>
#include <assert.h>

///External
#include <physfs.h>
#include <stb/stb_image.h>

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

	public:
		bool
			LoadPlugin
			(
				const string& fp_PluginFilePath,
				PluginInfo& fp_Plugin
			)
		{
			DYNLIB_HANDLE f_Handle;

			if (not (filesystem::exists(fp_PluginFilePath) and filesystem::is_regular_file(fp_PluginFilePath)))
			{
				resource_logger->LogAndPrint("Failed to locate DLL at: " + fp_PluginFilePath, "PluginManager", LogManager::LogLevel::Error);
				return false;
			}

			f_Handle = DYNLIB_LOAD(fp_PluginFilePath.c_str());
			resource_logger->LogAndPrint("Successfully located DLL at: " + fp_PluginFilePath, "PluginManager", LogManager::LogLevel::Debug);

			if (not f_Handle)
			{
				resource_logger->LogAndPrint("Failed to load plugin at path: " + fp_PluginFilePath, "PluginManager", LogManager::LogLevel::Error);
				return false;
			}
			
			resource_logger->LogAndPrint("Successfully loaded plugin at: " + fp_PluginFilePath, "PluginManager", LogManager::LogLevel::Debug);
			
			auto f_CreateFunc = (CreatePluginFunc)DYNLIB_GETSYM(f_Handle, "createPlugin");
			auto f_DestroyFunc = (DestroyPluginFunc)DYNLIB_GETSYM(f_Handle, "destroyPlugin");

			if (not f_CreateFunc or not f_DestroyFunc)
			{
				resource_logger->LogAndPrint("Failed to find CreatePlugin() or DestroyPlugin() functions in: " + fp_PluginFilePath, "PluginManager", LogManager::LogLevel::Error);
				DYNLIB_UNLOAD(f_Handle);
				return false;
			}
			
			resource_logger->LogAndPrint("Successfully located CreatePlugin() or DestroyPlugin() functions in: " + fp_PluginFilePath, "PluginManager", LogManager::LogLevel::Debug);

			fp_Plugin.Pwugin = unique_ptr<Plugin, DestroyPluginFunc>(f_CreateFunc(), f_DestroyFunc); //creates smrt poiner with destructor tied to it;
			fp_Plugin.Handle = f_Handle;

			return true;
		}
	};
}