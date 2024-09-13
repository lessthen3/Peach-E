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

#include <vector>
#include <string>
#include <variant>
#include <mutex>
#include <queue>
#include <map>

#include <glm/glm.hpp>

using namespace std;

namespace PeachCore
{
    enum class Drawable //enum wrapper for indicating to RenderingManager which asset to create
    {
        _Sprite2D,
        _TileSet,
        _ParallaxLayer,
        _AnimatedSprite2D,
        _Texture2D
    };

    enum class ShaderType // >w<
    {
        VertexShader,
        FragmentShader,
        ComputeShader,
        TesselationShader,
        GeometryShader
    };

    struct ShaderUniformUpdateData
    {
        string UniformName;
        variant
            <
            bool,
            int,
            float,
            glm::vec2,
            glm::vec3,
            glm::vec4,
            glm::mat3,
            glm::mat4,
            vector<glm::mat3>,
            vector<glm::mat4>,
            vector<float>,
            vector<bool>
            > UniformValue; //this can't possibly go wrong

    };

    struct CreateDrawableData //INDEX 0
    {
        string ResourceFilePath; //used for renderingmanager to call resourceloadingmanager on the loading thread
        string ObjectID;

        glm::vec2 InitialPosition;
        uint32_t LayerNumber;

        bool IsVisible;

        Drawable GraphicsType; //RenderingManager only needs to know the drawable type on creation
        map<ShaderType, string> ShaderFilePaths; // ShaderType : shader source file path
    };

    struct UpdateActiveDrawableData//INDEX 1
    {
        string ObjectID;

        glm::vec2 DeltaPosition; //keeps track of how much the position needs to change from this current frame by the time the next frame comes around
        uint32_t LayerNumber; //Keeps track of which layer/draw order of the drawable object
        
        bool IsVisible;
        bool IsQueuedForRemoval;
        
        vector<ShaderUniformUpdateData> UniformUpdates; //used to update uniform states after the ShaderProgram was passed in the CreateDrawableData step
    };

    struct DeleteDrawableData//INDEX 2
    {
        string ObjectID;
        //implement any additional cleanup logic based off of object type if needed
    };

    //USED BY MAIN THREAD TO SEND DRAWING COMMANDS TO THE RENDERING THREAD
    struct DrawCommand
    {
        //Index 0 --> CreateData, Index 1 --> UpdateData, Index 2 --> DeleteData, always. don't need a special typing system because we dont need anything else
        variant
            <
            vector<CreateDrawableData>, 
            vector<UpdateActiveDrawableData>, 
            vector<DeleteDrawableData>
            > DrawableData[3];                   //stores a list of all data needed to be updated that frame only need 3 spaces for our commands
    }; //implement a way to track what commands are going out on what frame, so implement a global frame tracker here for debugging purposes TOODODDODODODODODDOO

    //USED BY THE RENDERING THREAD TO SEND ANY RELEVANT DATA BACK TO THE MAIN THREAD
    struct Response
    {
        //queue<unique_ptr<PeachNode>> ListOfPeachNodesToAdd;

    };


    class CommandQueue 
    {
    public:
        CommandQueue() {}
        ~CommandQueue()
        {
           // pm_CommandQueue.empty(); //why not
        }

    private:
        queue<DrawCommand> pm_SendersQueue;
        queue<Response> pm_ResponsdersQueue;

        mutex Mutex;

        //////////////////////////////////////////////
         // Methods for handling pm_CommandQueue
         //////////////////////////////////////////////

    public:
        // Push a new command onto the queue
        void PushSendersQueue(const DrawCommand& command)
        {
            lock_guard<mutex> lock(Mutex);
            pm_SendersQueue.push(command);
        }

        // Pop the next command from the queue
        bool PopSendersQueue(DrawCommand& command) //returns true if empty, returns false if there are more commands to process--for use in while loops
        {
            lock_guard<mutex> lock(Mutex);

            if (pm_SendersQueue.empty())
                {return true;}

            command = move(pm_SendersQueue.front());
            pm_SendersQueue.pop();

            return false;
        }

        // Push a new command onto the queue
        void PushRespondersQueue(const Response& fp_Response)
        {
            lock_guard<mutex> lock(Mutex);
            pm_ResponsdersQueue.push(fp_Response);
        }

        // Pop the next command from the queue
        bool PopRespondersQueue(Response& fp_Response) //returns true if empty, returns false if there are more commands to process--for use in while loops
        {
            lock_guard<mutex> lock(Mutex);

            if (pm_ResponsdersQueue.empty())
                {return true;}

            fp_Response = move(pm_ResponsdersQueue.front());
            pm_ResponsdersQueue.pop();

            return false;
        }
    };
}