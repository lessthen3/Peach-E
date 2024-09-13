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

#include <mutex>
#include <string>
#include <queue>
#include <variant>


#include <nlohmann/json.hpp>

using namespace std;

namespace PeachCore {

    // Overloaded pattern implementation for std::visit
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    struct TextureData
    {
        //declaring texture data as a unique_ptr with the stbi_image_free function attached to its deleter
        unique_ptr<unsigned char> m_TextureByteData;

        int m_TextureWidth;
        int m_TextureHeight;
        int m_NumColourChannels;

        TextureData(unsigned char* ptr, const int w, const int h, const int c)
            : m_TextureByteData(ptr), m_TextureWidth(w), m_TextureHeight(h), m_NumColourChannels(c) {}

        TextureData() : m_TextureByteData(nullptr), m_TextureWidth(0), m_TextureHeight(0), m_NumColourChannels(0) {}
    };

    struct AudioData
    {
        unique_ptr<unsigned char[]> m_AudioByteData;  // Assuming a different free function if needed
        size_t m_Size;  // Size of the audio data

        AudioData(unsigned char* ptr, size_t sz)
            : m_AudioByteData(ptr), m_Size(sz) {}
    };


    struct LoadedResourcePackage
    {
        string PeachObjectID; //target object that this resource is for, unecessary but good for error checking

        variant //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
            <
            //unique_ptr<unsigned const char>, //used for parsins byte info that is supposed to be immutable, mainly for preference not really required -- NOT SURE IF NEEDED OR NOT
            TextureData, //used for parsing raw byte information, mainly for audio at the moment
            AudioData,
            unique_ptr<nlohmann::json> //used for parsing JSON metadata if required

            > ResourceData;

        LoadedResourcePackage(const string& fp_ID, TextureData& fp_TextureData)
            : PeachObjectID((fp_ID)), ResourceData(move(fp_TextureData)) {}

        LoadedResourcePackage(const string& fp_ID, AudioData& fp_AudioData)
            : PeachObjectID((fp_ID)), ResourceData(move(fp_AudioData)) {}

        LoadedResourcePackage(const string& fp_ID, unique_ptr<nlohmann::json> fp_JSONData)
            : PeachObjectID((fp_ID)), ResourceData(move(fp_JSONData)) {}

        LoadedResourcePackage() = default;
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
        queue<unique_ptr<LoadedResourcePackage>> pm_LoadedResourceQueue;
        mutex Mutex;

        //////////////////////////////////////////////
        // Methods for handling pm_LoadCommandQueue
        //////////////////////////////////////////////

    public:
        // Push a new command onto the queue
        void PushLoadCommandQueue(const LoadCommand& command) //Never supposed to be used by ResourceLoadingManager
        {
            lock_guard<mutex> lock(Mutex);
            pm_LoadCommandQueue.push(command);
        }

        // Pop the next command from the queue
        bool PopLoadCommandQueue(LoadCommand& command) //returns true if empty, returns false if there are more commands to process--for use in while loops
        {
            lock_guard<mutex> lock(Mutex);

            if (pm_LoadCommandQueue.empty())
                {return true;}

            command = move(pm_LoadCommandQueue.front());
            pm_LoadCommandQueue.pop();

            return false;
        }

        //////////////////////////////////////////////
        // Methods for handling pm_LoadedResourceQueue
        //////////////////////////////////////////////

        // Push a new command onto the queue
        bool PushLoadedResourcePackage(vector<unique_ptr<LoadedResourcePackage>>& fp_ListOfPackages) //used exclusively by ResourceLoadingManager, NO OTHER CLASS SHOULD EVER USE THIS
        {
            unique_lock<mutex> lock(Mutex, try_to_lock);
            //used for lazy pushing of LoadedPackages because i decided that loading assets while rendering and gameplay isnt a high priority, and this is better for level memory paging
            if (!lock.owns_lock()) 
            {
                return false;
            } // lock not acquired, return early
            
            for (auto& package : fp_ListOfPackages)
            {
                pm_LoadedResourceQueue.push(move(package));
            }
 
            return true;
        }


        // pass by ref since Rendering and Audio Manager pass a placeholder ptr
        bool PopLoadedResourceQueue(unique_ptr<LoadedResourcePackage>& fp_Package) //returns false if empty, returns true if there are more packages to process--for use in while loops
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