#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>

<<<<<<< HEAD:include/Peach-Core/2D/CollisionShape2D.h
#include "Physics2DManager.h"
#include "LogManager.h"
#include "PeachNode2D.h"
=======
<<<<<<< Updated upstream:src/Peach-core/include/Peach-Core2D/SceneTreeItems2D/Physics2D/CollisionShape2D.h
#include "../../../Managers/Physics2DManager.h"
#include "../../../Managers/LogManager.h"
#include "../../PeachNode2D.h"
=======
#include "../Managers/Physics2DManager.h"
#include "../Managers/LogManager.h"
#include "PeachNode2D.h"
>>>>>>> Stashed changes:include/Peach-Core/2D/CollisionShape2D.h
>>>>>>> 03203df36206f71282dd66e8e335c2b65abe7254:src/Peach-core/include/Peach-Core2D/SceneTreeItems2D/Physics2D/CollisionShape2D.h

#include "Sensor2D.h"

using namespace std;

namespace PeachCore {

    class CollisionShape2D 
    {
    public:
        // Initialize collision shape with nothing, so that it can be registered from the main thread to preserve more control and make responsibilites more clear
        CollisionShape2D() = default;

        ~CollisionShape2D() 
        {
            if (pm_IsValid) 
            {
                b2DestroyBody(pm_Body2D);
                pm_Body2D = b2_nullBodyId;
            }
        }

        // Accessor for the underlying Box2D body
        b2BodyId GetBody() const { return pm_Body2D; }

        // Update method if needed, e.g., for syncing with game object transforms
        void Update()
        {
            // Sync game object transform with physics body position
            // This would typically be called every game update tick
        }

        //IMPORTANT WE NEED TO ADD A METHO

        void CreatePhysicsBody(b2WorldId fp_World, b2BodyId& fp_Body2D, const glm::vec2& fp_Position, bool fp_IsDynamic = true)
        {
            b2BodyDef f_BodyDefinition = b2DefaultBodyDef();
            f_BodyDefinition.type = fp_IsDynamic ? b2_dynamicBody : b2_staticBody;
            f_BodyDefinition.position = b2Vec2({ fp_Position.x, fp_Position.y });

            fp_Body2D = b2CreateBody(fp_World, &f_BodyDefinition);
            pm_IsValid = true; //you arent allowed to access this manually anywhere
        }

        void CreateBoxCollisionShape2D(b2WorldId fp_World, const glm::vec2& fp_Position, const glm::vec2& fp_Size, bool fp_IsDynamic = true)
        {
            if(pm_IsValid) //Cleans up body if creating a new one using the recommended method by Box2D docs
            {
                b2DestroyBody(pm_Body2D);
                pm_Body2D = b2_nullBodyId;
                //need to destroy attached joints manually
            }

            CreatePhysicsBody(fp_World, pm_Body2D, fp_Position, fp_IsDynamic);

            b2Polygon f_Box = b2MakeBox(fp_Position.x, fp_Position.y);
            
            b2CreatePolygonShape(pm_Body2D, &CreateShapeDefinition(), &f_Box);
        }

        void CreateCircleCollisionShape2D(b2WorldId fp_World, const glm::vec2& fp_Position, const float fp_Radius, bool fp_IsDynamic = true)
        {
            if(pm_IsValid) //Cleans up body if creating a new one using the recommended method by Box2D docs
            {
                b2DestroyBody(pm_Body2D);
                pm_Body2D = b2_nullBodyId;
                //need to destroy attached joints manually
            }

            CreatePhysicsBody(fp_World, pm_Body2D, fp_Position, fp_IsDynamic);

            b2Circle f_Circle;
            f_Circle.center = static_cast<b2Vec2>( fp_Position.x, fp_Position.y );
            f_Circle.radius = fp_Radius;

            b2CreateCircleShape(pm_Body2D, &CreateShapeDefinition(), &f_Circle);
        }

        void CreateCapsuleShape(b2WorldId fp_World, const glm::vec2& fp_Center1, const glm::vec2& fp_Center2, const float fp_Radius, bool fp_IsDynamic = true) //YEEEEEEEEEEEEEEE V3 FEATURE
        {
            if (pm_IsValid) //Cleans up body if creating a new one using the recommended method by Box2D docs
            {
                b2DestroyBody(pm_Body2D);
                pm_Body2D = b2_nullBodyId;
                //need to destroy attached joints manually
            }
            //calculates the midpoint between the two capsule centers
            glm::vec2 f_Midpoint = glm::vec2((fp_Center1.x + fp_Center2.x) / 2, 
                                                                 (fp_Center1.y + fp_Center2.y) / 2 );

            CreatePhysicsBody(fp_World, pm_Body2D, f_Midpoint, fp_IsDynamic);

            b2Capsule f_Capsule;
            f_Capsule.center1 = static_cast<b2Vec2>( fp_Center1.x, fp_Center1.y );
            f_Capsule.center2 = static_cast<b2Vec2>( fp_Center2.x, fp_Center2.y );
            f_Capsule.radius = fp_Radius;

            b2CreateCapsuleShape(pm_Body2D, &CreateShapeDefinition(), &f_Capsule);
        }

        b2ShapeDef& CreateShapeDefinition(const float fp_Density = 1.0f, const float fp_Friction = 0.3f, const float fp_Bounciness = 0.2f)
        {
            b2ShapeDef f_ShapeDefinition = b2DefaultShapeDef();
            f_ShapeDefinition.density = fp_Density; //KABOOOOOOOOOOOOOOOOM
            f_ShapeDefinition.friction = fp_Friction; //default friction value
            f_ShapeDefinition.restitution = fp_Bounciness; // bouncy
            return f_ShapeDefinition;
        }

        private:
            b2BodyId pm_Body2D = b2_nullBodyId;  // Pointer to the Box2D body associated with this shape
            bool pm_IsValid = false;

            vector<Sensor2D> pm_ListOfCurrentlyAttachedSensors;
    };
}