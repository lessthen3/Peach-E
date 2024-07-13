#include "../../include/Managers/ResourceLoadingManager.h"

namespace PeachCore {
	ResourceLoadingManager::~ResourceLoadingManager()
	{

	}

	ResourceLoadingManager::ResourceLoadingManager()
	{

	}





	GLuint ResourceLoadingManager::LoadTextureData(const std::string& fp_ImagePath, OpenGLRenderer* fp_CurrentRenderer) //loads and registers data and returns the GLuint reference for the texture ID, possibly vao info too
	{
		GLuint f_TextureID;

		// Load image data (consider using stb_image or any similar library)
		int width, height, nrChannels;
		unsigned char* data = stbi_load(fp_ImagePath.c_str(), &width, &height, &nrChannels, 0);

		if (!data)
		{
			std::cerr << "Failed to load texture image!" << std::endl;
		}


		f_TextureID = fp_CurrentRenderer->RegisterTexture(data, width, height);

		stbi_image_free(data);
		return f_TextureID;
	}

		// Called by the rendering thread
	std::list<PendingResource> ResourceLoadingManager::FetchPendingResources() {
		std::lock_guard<std::mutex> lock(resourceMutex);
		//auto& resources = std::move(pm_ListOfResourcesThatNeedToBeLoaded); // Move the contents
		//pm_ListOfResourcesThatNeedToBeLoaded.clear(); // Clear the original list
		//return resources;
		return {};
	}
}