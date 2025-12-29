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

///STL
#include <string>
#include <vector>
#include <memory>

///PeachCore
#include "../Managers/StatusCodes.h"

///GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace PeachCore {

    using namespace std; //this should be here so i dont affect anybody who links against peach

    using PeachNodeID = uint64_t; //node id is , high 56 bits is an index and the low 8 bits are the node type uwu

    constexpr uint64_t PEACH_NODE_TYPE_MASK = 0xFFull;        // lower 8 bits
    constexpr uint64_t PEACH_NODE_INDEX_MASK = ~PEACH_NODE_TYPE_MASK;
    constexpr uint64_t PEACH_NODE_TYPE_BITS = 8U;

    constexpr uint64_t PEACH_NODE_NULL_ID = 0; //Represents no ID, or invalid ID

    static inline PeachNodeID
        MakeNodeID(uint64_t fp_Index, uint8_t fp_NodeType)
        noexcept
    {
        // index goes into the upper 56 bits, type in lower 8
        return (fp_Index << PEACH_NODE_TYPE_BITS) | static_cast<uint64_t>(fp_NodeType);
    }

    static inline uint8_t
        GetNodeType(PeachNodeID fp_NodeID)
        noexcept
    {
        return static_cast<uint8_t>(fp_NodeID & PEACH_NODE_TYPE_MASK);
    }

    static inline uint64_t
        GetNodeIndex(PeachNodeID fp_NodeID)
        noexcept
    {
        return fp_NodeID >> PEACH_NODE_TYPE_BITS;
    }

    enum PeachNodeType : uint8_t
    {
        Blank,
        Render,
        Physics,
        Audio
    };

    class PeachNode
    {
    public:
        virtual ~PeachNode();

        PeachNode(const string& fp_Name) 
        {
            m_Name = fp_Name;
        }

        PeachNode() {} //??????????????????

        bool IsPausable = true;
        bool IsVisible = true;
        bool IsActive = true;

        string m_Name; //this is the name set by the user inside the scene tree, so that a user script can call smth like GetNode("MyNamedNode") and find the appropriate thing

        PeachNodeID ID = PEACH_NODE_NULL_ID;
        PeachNodeID pm_ParentNode = PEACH_NODE_NULL_ID; //this is fine since te parent handle should never be null because the lifetime of the child is strictly tied to its parent

        vector<PeachNodeID> pm_Children; //don't need to explicitly clean this up since the destructors will clean up everything properly when the owning node exits scope

        void
            AddChild(PeachNodeID fp_Child)
        {
            pm_Children.push_back(fp_Child);
        }


        PeachNode* 
            GetPeachNode(); //returns a reference to the desired PeachNode

        bool 
            ReparentPeachNode(); //returns true if operation was successful, returns false otherwise

        void
            PrintTree();

        bool 
            IsInsideTree();

        [[nodiscard]] bool 
            HasPeachNode();

        void 
            GetViewPort(); //?????????? why does godot have this lmfao

        string
            GetPathInTree();

        PeachNode*
            FindChild(uint64_t fp_DesiredPeachNode); //returns first instance of child found matching the name

        PeachNode 
            Duplicate();
    };
}

namespace PeachCore {

    struct Transform2D
    {
    private:
        glm::vec2 pm_Position;
        float pm_Rotation;
        glm::vec2 pm_Scale;

    public:
        Transform2D() = default;

        Transform2D(const glm::vec2& fp_Position, float fp_Rotation, const glm::vec2& fp_Scale)
            : pm_Position(fp_Position), pm_Rotation(fp_Rotation), pm_Scale(fp_Scale) {
        }

        // Getters
        const glm::vec2& GetPosition() const { return pm_Position; }
        const float GetRotation() const { return pm_Rotation; }
        const glm::vec2& GetScale() const { return pm_Scale; }

        // Setters
        void
            SetPosition(const glm::vec2& fp_Position)
        {
            pm_Position = fp_Position;
        }

        void
            SetRotation(const float fp_Rotation)
        {
            pm_Rotation = fp_Rotation;
        }

        void
            SetScale(const glm::vec2& fp_Scale)
        {
            pm_Scale = fp_Scale;
        }

        //void SetTransform(const sf::Transform& fp_Transform) { pm_Transform = fp_Transform; }

        // Translate (move) by an offset
        //void Translate(const glm::vec2& fp_Offset) { pm_Position += fp_Offset; }

        // Rotate by an angle (in degrees)
        void
            Rotate(const float fp_Angle)
        {
            pm_Rotation += fp_Angle;
        }

        // pm_Scale uniformly or non-uniformly
        //void ScaleBy(const glm::vec2& fp_Factor) { pm_Scale.x *= fp_Factor.x; pm_Scale.y *= fp_Factor.y; }

        // Apply the transform to a point
        //glm::vec2 TransformPoint(const glm::vec2& fp_Point) {
        //    pm_Transform.translate(pm_Position);
        //    pm_Transform.rotate(pm_Rotation);
        //    pm_Transform.scale(pm_Scale);
        //    return pm_Transform.transformPoint(fp_Point);
        //}
    };

    class PeachNode2D : public PeachNode
    {
    public:
        virtual ~PeachNode2D();

        //PeachNode2D(){}
        PeachNode2D() :PeachNode() {}

        PeachNode2D(const string& fp_Name) : PeachNode(fp_Name) {};

        virtual void
            OnEnter() = 0;

        virtual void
            OnUpdate(float fp_TimeSinceLastFrame) = 0;

        virtual void
            OnConstantUpdate(float fp_TimeSinceLastFrame) = 0;

        virtual void
            OnExit() = 0;

        virtual void
            QueueRemoval() = 0; //queues for removal from scene tree at end of frame or whenever is convenient idk
    };
}

namespace PeachCore
{
    class Transform3D
    {
        glm::mat4 ProjectionMatrix;
        glm::mat4 GetModelViewMatrix;
        glm::mat4 ViewMatrix;
        glm::mat4 OrthographicMatrix;

    //public:
    //    Transform3D()
    //    {
    //        GetModelViewMatrix = glm::mat4();
    //        ProjectionMatrix = glm::mat4();
    //        ViewMatrix = glm::mat4();
    //        OrthographicMatrix = glm::mat4();
    //    }

    //    glm::mat4
    //        GetProjectionMatrix(float fov, float width, float height, float zNear, float zFar)
    //        const
    //    {
    //        float aspectRatio = width / height;
    //        //ProjectionMatrix.identity();
    //        //ProjectionMatrix.perspective(fov, aspectRatio, zNear, zFar);
    //        return ProjectionMatrix;
    //    }

    //    glm::mat4
    //        GetOrthographicMatrix(float fov, float width, float height, float zNear, float zFar)
    //        const
    //    {
    //        //OrthographicMatrix.identity();
    //        //OrthographicMatrix.orthoSymmetric(width, height, zNear, zFar);
    //        return OrthographicMatrix;
    //    }

    //    glm::mat4
    //        GetModelViewMatrix(const glm::vec3& fp_Rotation, glm::mat4 vMatrix)
    //        const
    //    {
    //        //GetModelViewMatrix.identity().translate(gameItem.getPosition())
    //        //    .rotateX(glm::radians(-fp_Rotation.x))
    //        //    .rotateY(glm::radians(-fp_Rotation.y))
    //        //    .rotateZ(glm::radians(-fp_Rotation.z))
    //        //    .scale(gameItem.getScale());

    //        return  glm::mat4(vMatrix*GetModelViewMatrix);
    //    }

    //    glm::mat4
    //        GetViewMatrix(const glm::vec3& fp_CameraPos, const glm::vec3& fp_Rsotation)
    //        const
    //    {
    //        //ViewMatrix.identity();

    //        //ViewMatrix.rotate(glm::radians(rotation.x), glm::vec3(1, 0, 0))
    //        //    .rotate(glm::radians(rotation.y), glm::vec3(0, 1, 0));

    //        //ViewMatrix.translate(-cameraPos.x, -cameraPos.y, -cameraPos.z);

    //        return ViewMatrix;
    //    }
    };

    struct PeachNode3D : public PeachNode
    {

    };
}