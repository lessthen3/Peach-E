#pragma once

#include <nlohmann/json.hpp>

#include "../Unsorted/PeachNode.h"
#include "../Peach-Core2D/Rendering2D/PeachCamera2D.h"

#include "../Peach-Core2D/SceneTreeItems2D/Geometry2D/Curve2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Geometry2D/Grid2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Geometry2D/Path2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Geometry2D/PathFinder2D.h"

#include "../Peach-Core2D/SceneTreeItems2D/Graphics2D/AnimationPlayer2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Graphics2D/ParallaxBackgroundPlayer.h"
#include "../Peach-Core2D/SceneTreeItems2D/Graphics2D/ParallaxLayer.h"
#include "../Peach-Core2D/SceneTreeItems2D/Graphics2D/Sprite2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Graphics2D/TileMap.h"
#include "../Peach-Core2D/SceneTreeItems2D/Graphics2D/TileSet.h"

#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/CharacterController2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/CollisionPolygon2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/CollisionSegment2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/CollisionShape2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/RayCast2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Sensor2D.h"

#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Joints2D/FixedDistanceJoint2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Joints2D/HingeJoint2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Joints2D/Joint2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Joints2D/MotorJoint2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Joints2D/MouseJoint2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Joints2D/PistonJoint2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Joints2D/StaticJoint2D.h"
#include "../Peach-Core2D/SceneTreeItems2D/Physics2D/Joints2D/WheelJoint2D.h"

using namespace std;

namespace PeachCore {

    class SerializationManager
    {
    public:
        static SerializationManager& Serializer() 
        {
            static SerializationManager serializer;
            return serializer;
        }
        ~SerializationManager();

    private:
        SerializationManager();
        SerializationManager(const SerializationManager&) = delete;
        SerializationManager& operator=(const SerializationManager&) = delete;


    public:

        PeachCamera2D* DeserializePeachCamera2DFromJSON(const string& fp_JSONData)
        {

        }

        Sprite2D* DeserializeSprite2DFromJSON(const string& fp_JSONData)
        {

        }

        TileSet* DeserializeTileSetFromJSON(const string& fp_JSONData)
        {

        }

        TileMap* DeserializeTileMapFromJSON(const string& fp_JSONData)
        {

        }

        CollisionShape2D* DeserializeCollisionShape2DFromJSON(const string& fp_JSONData)
        {

        }

        CollisionPolygon2D* DeserializeCollisionPolygon2DFromJSON(const string& fp_JSONData)
        {

        }

    };

}