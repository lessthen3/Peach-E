/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include <mutex>
#include <string>
#include <queue>
#include <variant>

namespace PeachCore {

    using namespace std;

    // Overloaded pattern implementation for std::visit
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    struct TextureData //POD structs dont use m_ 
    {
        //declaring texture data as a unique_ptr with the stbi_image_free function attached to its deleter
        unsigned char* TextureByteData;

        uint32_t TextureWidth;
        uint32_t TextureHeight;
        uint32_t NumColourChannels;

        explicit 
            TextureData
            (
                unsigned char* fp_RawTextureByteData, 
                const uint32_t fp_TextureWidth, 
                const uint32_t fp_TextureHeight, 
                const uint32_t fp_NumberOfColourChannels
            )
        {
            TextureByteData = fp_RawTextureByteData;
            TextureWidth = fp_TextureWidth;
            TextureHeight = fp_TextureHeight;
            NumColourChannels = fp_NumberOfColourChannels;
        }

    };

    struct AudioData
    {
        unique_ptr<unsigned char[]> m_AudioByteData;  // Assuming a different free function if needed
        size_t m_Size;  // Size of the audio data

        AudioData(unsigned char* ptr, size_t sz)
            : m_AudioByteData(ptr), m_Size(sz) {}
    };

    enum class LoadedDataType //TODO: probably add more descriptive types so that it describes formats possible or stuff that into the data structs
    {
        NoData,
        TextureData,
        AudioData
    };

    struct LoadedResourcePackage
    {
        string PeachObjectID; //target object that this resource is for, unecessary but good for error checking
        LoadedDataType Type = LoadedDataType::NoData;

        variant //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
            <
            //unique_ptr<unsigned const char>, //used for parsins byte info that is supposed to be immutable, mainly for preference not really required -- NOT SURE IF NEEDED OR NOT
            unique_ptr<TextureData>, //used for parsing raw byte information, mainly for audio at the moment
            unique_ptr<AudioData>
            //unique_ptr<nlohmann::json> //used for parsing JSON metadata if required WARNING: CHANGING TO JUST CEREAL SINCE WE DESERIALIZE IN CHUNKS AND SERIALIZE IN KNOWN CHUNKS

            > ResourceData;

        explicit 
            LoadedResourcePackage
            (
                const string& fp_ID, 
                unique_ptr<TextureData> fp_TextureData
            )
        {
            PeachObjectID = fp_ID; 
            ResourceData = move(fp_TextureData);
            Type = LoadedDataType::TextureData;
        }

        explicit 
            LoadedResourcePackage
            (
                const string& fp_ID, 
                unique_ptr<AudioData> fp_AudioData
            )
        {
            PeachObjectID = fp_ID; 
            ResourceData = move(fp_AudioData);
            Type = LoadedDataType::AudioData;
        }
    };

    struct LoadCommand //actual command that gets passed strictly one way from RenderingManager or AudioManager --> ResourceLoadingManager via their respective queues
    {
        string PeachObjectID;
        string PathToResource; //i wanna make this const so bad but it really does not like const objects in the move command >w<, we'll handle it const externally to avoid errors
    };


    class LoadingQueue //class used for creating thread safe loading queues so that assets can be loaded async and parallel to program execution
    {
    public:
        LoadingQueue() {}
        ~LoadingQueue()
        {
            //pm_LoadCommandQueue.clear(); //why not
            //pm_LoadedResourceQueue.clear();
        }

    private:
        queue<LoadCommand> pm_LoadCommandQueue;
        queue<LoadedResourcePackage> pm_LoadedResourceQueue;
        mutex Mutex;

        //////////////////////////////////////////////
        // Methods for handling pm_LoadCommandQueue
        //////////////////////////////////////////////

    public:
        // Push a new command onto the queue
        bool 
            PushLoadCommandQueue(const LoadCommand& command) //Never supposed to be used by ResourceLoadingManager
        {
            unique_lock<mutex> lock(Mutex);

            if (not lock.owns_lock())
            {
                return false;
            } // lock not acquired, return early

            pm_LoadCommandQueue.push(command);

            return true;
        }

        // Pop the next command from the queue
        bool 
            PopLoadCommandQueue(LoadCommand& command) //returns true if empty, returns false if there are more commands to process--for use in while loops
        {
            lock_guard<mutex> lock(Mutex);

            if (pm_LoadCommandQueue.empty())
            {
                return true;
            }

            command = move(pm_LoadCommandQueue.front());
            pm_LoadCommandQueue.pop();

            return false;
        }

        //////////////////////////////////////////////
        // Methods for handling pm_LoadedResourceQueue
        //////////////////////////////////////////////

        // Push a new command onto the queue
        bool 
            PushLoadedResourcePackages(vector<LoadedResourcePackage>& fp_ListOfPackages) //used exclusively by ResourceLoadingManager, NO OTHER CLASS SHOULD EVER USE THIS
        {
            unique_lock<mutex> lock(Mutex, try_to_lock);
            //used for lazy pushing of LoadedPackages because i decided that loading assets while rendering and gameplay isnt a high priority, and this is better for level memory paging
            if (not lock.owns_lock()) 
            {
                return false;
            } // lock not acquired, return early
            
            for (auto& package : fp_ListOfPackages)
            {
                pm_LoadedResourceQueue.push(move(package));
            }
            
            //fp_ListOfPackages.clear(); //clear heap

            return true;
        }


        // pass by ref since Rendering and Audio Manager pass a placeholder ptr
        bool 
            PopLoadedResourceQueue(LoadedResourcePackage& fp_Package) //returns false if empty, returns true if there are more packages to process--for use in while loops
        {
            lock_guard<mutex> lock(Mutex);

            if (pm_LoadedResourceQueue.empty())
            {
                return false;
            }

            fp_Package = move(pm_LoadedResourceQueue.front());
            pm_LoadedResourceQueue.pop();

            return true;
        }
    };
}