#pragma once


#include <stb_image.h>

//#include <windows.h>
//#include <GL/glew.h>
//#include <gl/GL.h>
#include "../Unsorted/OpenGLRenderer.h"

//#include <map>
#include <mutex>
#include <list>
//#include <string>
//#include "LogManager.h"
//



namespace PeachCore {

	struct PendingResource {
		std::string Type;
		std::vector<unsigned char> Data;
		int Width, Height; //used for textures
	};

	class ResourceLoadingManager {
	public:
		static ResourceLoadingManager& ResourceLoader() {
			static ResourceLoadingManager resourceloader;
			return resourceloader;
		}

		~ResourceLoadingManager();

	public:
		std::mutex resourceMutex;

	private:
		ResourceLoadingManager();

		ResourceLoadingManager(const ResourceLoadingManager&) = delete;
		ResourceLoadingManager& operator=(const ResourceLoadingManager&) = delete;




	public:
		GLuint LoadTextureData(const std::string& fp_ImagePath, OpenGLRenderer* fp_CurrentRenderer);

		
		std::list<PendingResource> FetchPendingResources();

	private:
		std::list<PendingResource> pm_ListOfResourcesThatNeedToBeLoaded;
	};

}