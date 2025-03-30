/*******************************************************************
 *                                        Peach Editor v0.0.7
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                 Peach Editor is an open source editor for Peach-E
********************************************************************/
#pragma once

#include "../../include/Peach-Core/Managers/LogManager.h"

#include "../../include/Peach-Core/General/CommandQueue.h"
#include "../../include/Peach-Core/General/LoadingQueue.h"

#include <physfs.h>

namespace PeachEditor {

	//////////////////////////////////////////////
	// Public Methods
	//////////////////////////////////////////////
	class PeachEditorResourceLoadingManager 
	{
	//////////////////////////////////////////////
	// Public Destructor
	//////////////////////////////////////////////
	public:
		~PeachEditorResourceLoadingManager();
	//////////////////////////////////////////////
	// Singleton Instance
	//////////////////////////////////////////////
	public:
		static PeachEditorResourceLoadingManager& PeachEditorResourceLoader() 
		{
			static PeachEditorResourceLoadingManager peach_editor_resource_loader;
			return peach_editor_resource_loader;
		}
	//////////////////////////////////////////////
	// Private Constructor
	//////////////////////////////////////////////
	private:
		PeachEditorResourceLoadingManager();

		PeachEditorResourceLoadingManager(const PeachEditorResourceLoadingManager&) = delete;
		PeachEditorResourceLoadingManager& operator=(const PeachEditorResourceLoadingManager&) = delete;

	//////////////////////////////////////////////
	// Private Members
	//////////////////////////////////////////////
	private:
		shared_ptr<PeachCore::LoadingQueue> pm_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager
		shared_ptr<PeachCore::LoadingQueue> pm_DrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

		unsigned int pm_AudioQueueReferenceCount = 0;
		unsigned int pm_DrawableQueueReferenceCount = 0;

		vector<unique_ptr<PeachCore::LoadedResourcePackage>> pm_WaitingFullyLoadedResourcePackages;

		unique_ptr<PeachCore::LogManager> editor_resource_logger = nullptr;

	//////////////////////////////////////////////
	// Public Members
	//////////////////////////////////////////////
	public:
		mutex PeachEditorResourceMutex;

	//////////////////////////////////////////////
	// Public Methods
	//////////////////////////////////////////////
	public:
		bool
			InitializeLogger
			(
				const string& fp_LogOutputDirectory,
				shared_ptr<PeachCore::Console> fp_Console
			);

		shared_ptr<PeachCore::LoadingQueue> 
			GetAudioResourceLoadingQueue();

		shared_ptr<PeachCore::LoadingQueue> 
			GetDrawableResourceLoadingQueue();

	/////////////////////////////////////////////
	// Private Methods
	//////////////////////////////////////////////
	public: //PUBLIC FOR TESTING
		bool 
			TryPushingLoadedResourcePackage(unique_ptr<PeachCore::LoadedResourcePackage> fp_LoadedPackage);
		bool 
			LoadTextureFromSpecifiedFilePath(const string& fp_FilePath);
	};

}
