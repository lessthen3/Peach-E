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

#include "../Scene-Items/2D/CollisionSegment2D.h"
#include "../Scene-Items/2D/CollisionPolygon2D.h"
#include "../Scene-Items/2D/CollisionShape2D.h"

#include <unordered_map>

#include <semaphore>
#include <latch>

namespace PeachCore {

    enum CollisionShapeType
    {
        Box,
        Circle,
        Capsule,
        RoundedBox,
        Polygon // For future implementation
    };

    enum SensorType //idk how im gonna let users/me define sensors and use built in functions like IsOnFloor or IsOnWall or something. But I think allowing users/me to define specific sensors, and do their own thing
    {                           //maybe we can have a preset character prefab that has a preset IsOnWall, IsOnFloor, and IsOnAnimatableBody(platform) 
        FloorSensor,
        WallSensor, // Additional sensors can be defined here
        PlatformSensor
    };

    /*
    This is a bit mask since a single collision object can be in multiple layers or no layers, 
    so we can store every layer in a single uint32_t for ease and fast bitwise comparisons
    */
    enum CollisionLayer : uint32_t //Box2D supports up 32 layers for collision filtering
    {
        Layer_1 = 1 << 0, Layer_2 = 1 << 1, Layer_3 = 1 << 2, Layer_4 = 1 << 3,
        Layer_5 = 1 << 4, Layer_6 = 1 << 5, Layer_7 = 1 << 6, Layer_8 = 1 << 7,
        Layer_9 = 1 << 8, Layer_10 = 1 << 9, Layer_11 = 1 << 10, Layer_12 = 1 << 11,
        Layer_13 = 1 << 12, Layer_14 = 1 << 13, Layer_15 = 1 << 14, Layer_16 = 1 << 15,

        Layer_17 = 1 << 16, Layer_18 = 1 << 17, Layer_19 = 1 << 18, Layer_20 = 1 << 19,
        Layer_21 = 1 << 20, Layer_22 = 1 << 21, Layer_23 = 1 << 22, Layer_24 = 1 << 23,
        Layer_25 = 1 << 24, Layer_26 = 1 << 25, Layer_27 = 1 << 26, Layer_28 = 1 << 27,
        Layer_29 = 1 << 28, Layer_30 = 1 << 29, Layer_31 = 1 << 30, Layer_32 = 1U << 31,

        NO_LAYER = 0,

        ALL_LAYERS = 
        Layer_1 | Layer_2 | Layer_3 | Layer_4 | Layer_5 | Layer_6 | Layer_7 | Layer_8 | Layer_9 | Layer_10 | Layer_11 | Layer_12 |
        Layer_13 | Layer_14 | Layer_15 | Layer_16 | Layer_17 | Layer_18 | Layer_19 | Layer_20 | Layer_21 | Layer_22 | Layer_23 | Layer_24 |
        Layer_25 | Layer_26 | Layer_27 | Layer_28 | Layer_29 | Layer_30 | Layer_31 | Layer_32
    };

    //////////////////////////////////////////////
    // ResourceManager word size
    //////////////////////////////////////////////

    struct PhysicsCommand
    {
        uint32_t node_id;       // 4 bytes
        uint16_t opcode;        // 2 bytes
        uint16_t reserved;      // 2 bytes (alignment or flags)
        uint64_t operand;       // 8 bytes
    };

    ////////////////////////////////////////////////
    // PhysicsManager Class
    ////////////////////////////////////////////////
    class PhysicsManager 
    {

    ////////////////////////////////////////////////
    // Private Constructor & Destructor
    ////////////////////////////////////////////////
    private:
        ~PhysicsManager() = default;
        PhysicsManager() = default;
        //{
        //    //in box2D 3.0 destroying the world automatically cleans up all resources linked to the world including: bodies, joints and shapes
        //    b2DestroyWorld(pm_World);
        //    pm_World = b2_nullWorldId; //what is C
        //}

        PhysicsManager(const PhysicsManager&) = delete;
        PhysicsManager& operator=(const PhysicsManager&) = delete;

        PhysicsManager(PhysicsManager&&) = delete;
        PhysicsManager& operator=(PhysicsManager&&) = delete;

    ////////////////////////////////////////////////
    // Singleton Instance
    ////////////////////////////////////////////////
    public:
        static PhysicsManager& get_single()
        {
            static PhysicsManager physics_manager;
            return physics_manager;
        }

    ////////////////////////////////////////////////
    // Private Members
    ////////////////////////////////////////////////
    private:
        b2WorldId pm_World;

        unordered_map<string, b2BodyId> pm_Bodies;
        unordered_map<string, b2Vec2*> pm_CurrentPositionOfAllBodies;

        glm::vec2 pm_WorldOrigin = glm::vec2(0.0f, 0.0f); // Current world origin in meters
        //This conversion maps [0 pixels, 5000 pixels] --> [0 meters, 50 meters] which aligns well with the size of current monitors and Box2D's preferred size range
        const float PIXELS_PER_METER = 100.0f;  // Pixels to meters conversion factor, 100 pixels : 1 meter

        const float PHYSICS_ORIGIN_MAXIMUM_PLAYER_DISTANCE = ConvertMetersToPixels(2000); //should be under 2km, converts 2km to pixels

        unique_ptr<Logger> physics_logger;

        //////////////////// Thread Syncro Stuff ////////////////////

        atomic<bool> pm_IsRunning = true; //this doesn't need to be atomic but whatevs, or even needed tbh but probs helpful for the while loop maybes
        atomic<bool> pm_IsInitialized = false;

        binary_semaphore pm_PhysicsSemaphore{ 0 }; // starts locked (zero tickets)

    ////////////////////////////////////////////////
    // Public Members
    ////////////////////////////////////////////////
    public:

    ////////////////////////////////////////////////
    // Public Methods
    ////////////////////////////////////////////////
    public:

        void
            PhysicsLoop
            (
                const string& fp_LogOutputDirectory,
                latch& fp_InitLatch
            );

        void
            RequestPhysicsFrame();

        void
           Stop();

        bool
            InitializePhysicsEngine
            (
                const string& fp_LogOutputDirectory,
                const float fp_GravityX = 0.0f,
                const float fp_GravityY = -9.8f,
                const bool fp_Is3D = false
            )
        {
            physics_logger = make_unique<Logger>();
            if (not physics_logger->Initialize(ThreadName::PhysicsThread, fp_LogOutputDirectory, "PhysicsManager", Logger::LogLevel::ALL_LOGS))
            {
                PrintError("PhysicsManager failed to initialize the physics_thread logger >w<");
                return false;
            }
            physics_logger->Debug("PhysicsLogger successfully initialized", "PhysicsManager");

            b2Vec2 f_Gravity = { fp_GravityX, fp_GravityY };
            b2WorldDef f_WorldDefinition = b2DefaultWorldDef();
            f_WorldDefinition.gravity = (f_Gravity);

            //BOX2D WILL TRY TO SLEEP AND WAKE UP OBJECTS THAT HAVENT BEEN PROCESSED IN A WHILE
            // IT WILL DYNAMICALLY WAKE THEM UP IF A SLEEPING OBJECT IS COLLIDED WITH
            // WE CAN ALSO DO IT MANUALLY IF DESIRED TO DO SO
            //OPTIONAL, APPARENTLY INCREASES PERFORMANCE
            //f_WorldDefinition.enableSleep = false;

            pm_World = b2CreateWorld(&f_WorldDefinition);

            return true;
        }

        void RegisterCollisionPolygon2D()
        {

        }

        void RegisterCollisionShape2D()
        {

        }
                                                                                 //Box2D only works best with world sizes less than 2 km, so if we want a bigger world, shifiting the origin is the most ideal solution
        bool CheckIfWorldOriginNeedsToBeShifted() //checks if the player character has moved too far outside of the physics world, and shifts the origin to adjust for weird behaviour caused by sizing mistmatch
        {
            // Example: Check player's distance from origin
            glm::vec2 f_PlayerPosition = GetBodyPosition("player"); //measured in pixels, getbodyposition gets the position we stored when the collision object was added idk we'll figure it out

            //if (glm::length(f_PlayerPosition) > PHYSICS_ORIGIN_MAXIMUM_PLAYER_DISTANCE) //measured in pixels
            //{
            //    b2World_ShiftOrigin(b2Vec2(ConvertPixelsToMeters(f_PlayerPosition.x), ConvertPixelsToMeters(f_PlayerPosition.y))); //needs to be converted to Meters for Box2D
            //    return true;
            //}

            return false;
        }

        //used to convert pixel coordinates to physics world coordinates, since as the docs say "DO NOT USE PIXELS."
        //Moving Objects work best with 0.1 to 10 meters, static work well up to 50 meters.
        float ConvertPixelsToMeters(const float fp_Pixels)
            const
        {
            return fp_Pixels / PIXELS_PER_METER;
        }

        float ConvertMetersToPixels(const float fp_Meters)
            const
        {
            return fp_Meters * PIXELS_PER_METER;
        }

        void
            RequestPhysicsWorldStep()
        {
            pm_PhysicsSemaphore.release(); // Gives 1 ticket, wakes render thread
        }

        // Step World
        void 
            Step(const float fp_TimeStep, const int fp_VelocityIterations = 8, const int fp_PositionIterations = 3)
        {
            //pm_World->Step(fp_TimeStep, fp_VelocityIterations, fp_PositionIterations);
            b2World_Step(pm_World, fp_TimeStep, fp_VelocityIterations);
        }


        //DOUBLE CHECK THIS METHOD FOR PROPER FUNCTIONING, UNTESTED!
        // Get the position of a body
        glm::vec2 GetBodyPosition(const string& fp_ID)
        {
            if (pm_Bodies.find(fp_ID) != pm_Bodies.end())
            {
                b2Vec2 f_CurrentBodyPosition = b2Body_GetPosition(pm_Bodies[fp_ID]);
                return glm::vec2(f_CurrentBodyPosition.x, f_CurrentBodyPosition.y);
            }
            return glm::vec2();
        }

        unordered_map<string, b2Vec2*>& GetCurrentPositionOfAllBodies()
        {
            return pm_CurrentPositionOfAllBodies;
        }

        //DOUBLE CHECK THIS METHOD FOR PROPER FUNCTIONING, UNTESTED!
        // Delete a body
        void DeleteBody(const string& fp_ID) 
        {
            if (pm_Bodies.find(fp_ID) != pm_Bodies.end())
            {
                b2DestroyBody(pm_Bodies[fp_ID]);
                pm_Bodies[fp_ID] = b2_nullBodyId; //NULL THOSE POINTERS BOYZ
                pm_Bodies.erase(fp_ID);
            }
        }


        //THIS SHOULD WORK PROPERLY I HOPE
        // Set collision filtering for all attached shapes to a desired body
        void SetCollisionFiltering(b2BodyId fp_Body, const CollisionLayer fp_CollisionLayer, const CollisionLayer fp_CollisionLayerMask)
        {
            //GET ALL JOINTS AND SHAPES ATTACHED TO THE BODYID PASSED IN
            const unsigned int f_ShapeCount = b2Body_GetShapeCount(fp_Body);
            //const unsigned int f_JointCount = b2Body_GetJointCount(fp_Body);

            vector<b2ShapeId> f_ListOfShapeIDs;
            f_ListOfShapeIDs.reserve(f_ShapeCount);

           /* vector<b2JointId> f_ListOfJointIDs;
            f_ListOfJointIDs.reserve(f_JointCount);*/

            int f_ShapeReturnCount = b2Body_GetShapes(fp_Body, f_ListOfShapeIDs.data(), f_ShapeCount);
           // int f_JointReturnCount = b2Body_GetJoints(fp_Body, f_ListOfJointIDs.data(), f_JointCount);

            //CREATE NEW FILTER FOR ALL JOINTS AND SHAPES TO OBTAIN
            b2Filter f_NewFilter = b2DefaultFilter();

            f_NewFilter.categoryBits = fp_CollisionLayer;
            f_NewFilter.maskBits = fp_CollisionLayerMask;

            //LOOP THROUGH ALL SHAPES ATTACHED TO THE BODY PASSED IN
            for(int index = 0; index < f_ListOfShapeIDs.size() - 1; index++) //currently only handles shapes
            {
                b2ShapeId f_CurrentShapeID = f_ListOfShapeIDs[index];
                b2Shape_SetFilter(f_CurrentShapeID, f_NewFilter);
            }
        }

        // Attaches generic user-defined sensors in place of a traditional isonfloor or equivalent method. I feel this approach is not complicated and gives more power to designers
        b2ShapeId AttachSensor(b2BodyId& fp_Body, string& fp_SensorType, const glm::vec2& fp_Position, const glm::vec2& fp_Size)
        {
            b2Polygon f_Box = b2MakeBox(fp_Position.x, fp_Position.y);

            b2ShapeDef f_ShapeDefinition = b2DefaultShapeDef();
            f_ShapeDefinition.isSensor = true;
            f_ShapeDefinition.userData = static_cast<void*>(&fp_SensorType);  // Can be used to identify user generated sensors

            return b2CreatePolygonShape(fp_Body, &f_ShapeDefinition, &f_Box);
        }

        //NOT SURE IF I WANT AN EXPLICIT ISONFLOOR METHOD, I THINK LETTING THE USER DEFINE THEIR OWN ISONFLOOR USING SENSORS IS BETTER
        // I THINK SETTING UP CALLBACKS WITH CREATED SENSOR TYPES
        // Check if a body is on the floor (to be called in a f_Contact listener or similar) THESE METHODS ARE WRITTEN ASSUMING WE'RE USING TWO FIXTURES TO LISTEN
        //bool IsOnFloor(b2BodyId fp_Body)
        //{
        //    for (b2ContactEdge* ce = fp_Body->GetContactList(); ce; ce = ce->next)
        //    {
        //        b2Contact* f_Contact = ce->contact;

        //        if (f_Contact->IsTouching())
        //        {
        //            // Assuming the floor sensor fixture user data is set to "FloorSensor"
        //            b2Fixture* f_FixtureA = f_Contact->GetFixtureA();
        //            b2Fixture* f_FixtureB = f_Contact->GetFixtureB();

        //            if ((f_FixtureA->IsSensor() && static_cast<void*>(f_FixtureA->GetUserData().pointer) == FloorSensor) ||
        //                (f_FixtureB->IsSensor() && static_cast<void*>(f_FixtureB->GetUserData().pointer) == FloorSensor))
        //            {
        //                return true;
        //            }
        //        }
        //    }
        //    return false;
        //}
    };

} // namespace PeachCore






/////////////////////////////////////////// TODO(maybe):
        /////////////////// add a 2nd frame ready function for caching variables so that all initailization stuff can be done, actually wait, we can just make a function called
        /////////////////// Initialize() which is called first for every system. then LateInitialize() for variable caching at he beginning of a scene if needed.