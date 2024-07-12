#pragma once


//#include "../Unsorted/OpenGLRenderer.h"

#include <map>
#include <vector>
#include <list>
#include <mutex>
#include <future>
#include <string>
#include "LogManager.h"

#include <stb.h>


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

	private:
		ResourceLoadingManager() = default;

		ResourceLoadingManager(const ResourceLoadingManager&) = delete;
		ResourceLoadingManager& operator=(const ResourceLoadingManager&) = delete;




	public:
		//GLuint LoadTextureData(const std::string& fp_ImagePath, OpenGLRenderer* fp_CurrentRenderer) //loads and registers data and returns the GLuint reference for the texture ID, possibly vao info too
		//{
		//	//std::future<GLuint> f_TextureID;

		//	// Load image data (consider using stb_image or any similar library)
		//	int width, height, nrChannels;
		//	unsigned char* data = stbi_load(fp_ImagePath.c_str(), &width, &height, &nrChannels, 0);

		//	if (!data)
		//	{
		//		std::cerr << "Failed to load texture image!" << std::endl;

		//		//return (std::future<GLuint>)((GLuint) 1);
		//	}


		//	//f_TextureID = fp_CurrentRenderer->RegisterTexture(data, width, height);

		//	stbi_image_free(data);
		//	return 0;
		//}

		// Called by the rendering thread
		std::list<PendingResource> FetchPendingResources() {
			//std::lock_guard<std::mutex> lock(resourceMutex);
			//auto& resources = std::move(pm_ListOfResourcesThatNeedToBeLoaded); // Move the contents
			//pm_ListOfResourcesThatNeedToBeLoaded.clear(); // Clear the original list
			//return resources;
		}




	public:
		std::mutex resourceMutex;


	private:
		std::list<PendingResource> pm_ListOfResourcesThatNeedToBeLoaded;
	};

}