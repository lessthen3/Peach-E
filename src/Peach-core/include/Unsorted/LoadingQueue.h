////////////////////////////////////////////////////////
/***                             [Peach Core]                                ***/
////////////////////////////////////////////////////////
/***                            [Version 0.0.01]                             ***/
////////////////////////////////////////////////////////
/***  Copyright(c) 2024-present Ranyodh Singh Mandur  ***/
/***                               MIT License                                ***/
/***         Documentation: TBD                                          ***/
/*** GitHub: https://github.com/iLoveJohnFish/Peach-E ***/
/////////////////////////////////////////////////////////
#pragma once

#include <mutex>
#include <string>
#include <queue>
#include <variant>

#include <SFML/Graphics.hpp>

using namespace std;

namespace PeachCore {

    struct LoadedResourcePackage
    {
        string ObjectID; //target object that this resource is for, unecessary but good for error checking
        variant //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
            <
            //unique_ptr<unsigned const char>, //used for parsins byte info that is supposed to be immutable, mainly for preference not really required -- NOT SURE IF NEEDED OR NOT
            unique_ptr<unsigned char>, //used for parsing raw byte information, mainly for audio at the moment
            unique_ptr<sf::Texture>, //SFML loads textures in as a single wrapped unit
            unique_ptr<string> //used for parsing JSON metadata if required

            > ResourceData;

        LoadedResourcePackage(const string& fp_ID, unique_ptr<sf::Texture> tex): ObjectID(fp_ID), ResourceData(move(tex)) {}
        LoadedResourcePackage(const string& fp_ID, unique_ptr<unsigned char> fp_RawByteData) : ObjectID(fp_ID), ResourceData(move(fp_RawByteData)) {}
        LoadedResourcePackage(const string& fp_ID, unique_ptr<string> fp_JSONData) : ObjectID(fp_ID), ResourceData(move(fp_JSONData)) {}
        LoadedResourcePackage() = default;
    };

    struct LoadCommand //actual command that gets passed strictly one way from RenderingManager or AudioManager --> ResourceLoadingManager via their respective queues
    {
        string ObjectID;
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
                {return false;} // lock not acquired, return early
            
            for (auto& package : fp_ListOfPackages)
                {pm_LoadedResourceQueue.push(move(package));}
 
            return true;
        }


        // pass by ref since Rendering and Audio Manager pass a placeholder ptr
        bool PopLoadedResourceQueue(unique_ptr<LoadedResourcePackage>& fp_Package) //returns false if empty, returns true if there are more packages to process--for use in while loops
        {
            lock_guard<mutex> lock(Mutex);

            if (pm_LoadedResourceQueue.empty())
                {return false;}

            fp_Package = move(pm_LoadedResourceQueue.front());
            pm_LoadedResourceQueue.pop();

            return true;
        }
    };
}