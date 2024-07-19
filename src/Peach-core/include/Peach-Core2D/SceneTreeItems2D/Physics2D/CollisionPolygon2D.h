#pragma once

#include <CGAL/Cartesian/point_constructions_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/partition_2.h>


#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <stdexcept>
#include "../../../Managers/LogManager.h"
#include "../../../Managers/Physics2DManager.h"

using namespace std;

namespace PeachCore {

    typedef CGAL::Exact_predicates_inexact_constructions_kernel                                                         Kernel;
    typedef CGAL::Partition_traits_2<Kernel>                                                                                            Traits;
    typedef Traits::Point_2                                                                                                                         Point_2;
    typedef Traits::Polygon_2                                                                                                                     Polygon_2;

    typedef list<Polygon_2>                                                                                                                   Polygon_List;

    class CollisionPolygon2D {
    public:
        CollisionPolygon2D() = default;

        ~CollisionPolygon2D() 
        {
            if (pm_IsValid) //Cleans up body if creating a new one using the recommended method by Box2D docs
            {
                b2DestroyBody(pm_Body2D);
                pm_Body2D = b2_nullBodyId;
                //need to destroy attached joints manually
            }
        }

        //Algorithm will work for simple non self-intersecting polygons defined by a cloud of points
        //convexness is not strictly required, however co-linear points might fuck up the algorithm/slow it down, so we remove them from the list before performing the polygonal decomposition
        //we also check for a minimum of 3 vertices before doing ANY operation on the given set of vertices since those are degenerate cases, and u especially are a degenerate for trying to pass it
        //might have some imprecision for very small or very large values, but whatever floating point arithmetic can only do so much Xd
        bool CreatePolygonBody(const b2WorldId& fp_World, const glm::vec2& fp_Position, const vector<glm::vec2>& fp_Vertices, bool fp_IsDynamic = true) 
        {
            if (fp_Vertices.size() < 3)
            {
                LogManager::MainLogger().Warn("Tried to create a CollisionPolygon2D with less than 3 vertices", "CollisionPolygon2D");
                return false;
            }

            if (pm_IsValid) //Cleans up body if creating a new one using the recommended method by Box2D docs
            {
                b2DestroyBody(pm_Body2D);
                pm_Body2D = b2_nullBodyId;
                //need to destroy attached joints manually
            }

            //create one body for all shapes to be attached too
            CreatePhysicsBody(fp_World, pm_Body2D, fp_Position, fp_IsDynamic); //if im gonna mutate a variable, its going to be all in one place so its easy to read

            vector<b2Vec2> f_OrderedVertices = AssertWindingOrderCCW(fp_Vertices); //ensures that the points being passed to the CGAL algorithm are ordered appropriately

            //handles case where polygon decomposition isn't required
            if (fp_Vertices.size()  <= 8)
            {



            }
            //mmmmmm yummy math I don't have to write
            else
            {
                Polygon_List f_ConvexPolygons;
                //returns false immediately if the vertices are not able to be decomposed into a set of convex polygons
                if (!DecomposePossiblyConcavePolygon(f_OrderedVertices, f_ConvexPolygons))
                {
                    return false;
                }

                //we create a body, and bodydefintion for each polygon making up the cloud of vertices selected by the user/me
                //IK HOW TO DO IT
                for (const auto& poly : f_ConvexPolygons)
                {
                    vector<b2Vec2> f_b2Vertices;

                    for (const auto& v : poly)
                    {
                        f_b2Vertices.push_back(b2Vec2(v.x(), v.y()));
                    }

                    b2Polygon f_Polygon;
                    //TODO ACTUALLY CREATE POLYGONS, BOX2D ALLOWS UP TO ONLY 8 VERTICES MAX FOR EACH POLYGON
                    //THESE TWO METHODS ARE THE GENERAL OUTLINE TO HOW TO ADD EACH SHAPE TO THE CURRENT BODY
                   /* polygonShape.Set(b2Vertices.data(), b2Vertices.size());

                    AttachShape(polygonShape);*/
                }
            }

            return true;
        }

        void CreatePhysicsBody(b2WorldId fp_World, b2BodyId& fp_Body2D, const glm::vec2& fp_Position, bool fp_IsDynamic = true)
        {
            b2BodyDef f_BodyDefinition = b2DefaultBodyDef();
            f_BodyDefinition.type = fp_IsDynamic ? b2_dynamicBody : b2_staticBody;
            f_BodyDefinition.position = { fp_Position.x, fp_Position.y };

            fp_Body2D = b2CreateBody(fp_World, &f_BodyDefinition);
            pm_IsValid = true; //you arent allowed to access this manually anywhere
        }

        b2ShapeDef CreateShapeDefinition(const float fp_Density = 1.0f, const float fp_Friction = 0.3f, const float fp_Bounciness = 0.2f)
        {
            b2ShapeDef f_ShapeDefinition = b2DefaultShapeDef();
            f_ShapeDefinition.density = fp_Density; //KABOOOOOOOOOOOOOOOOM
            f_ShapeDefinition.friction = fp_Friction; //default friction value
            f_ShapeDefinition.restitution = fp_Bounciness; // bouncy
            return f_ShapeDefinition;
        }

        bool DecomposePossiblyConcavePolygon(vector<b2Vec2>& fp_InputVertices, Polygon_List& fp_OutputPolygons)
        {
            vector<b2Vec2> f_OptimizedVertices = RemoveCollinearPoints(fp_InputVertices);

            Polygon_2 f_Polygon;

            //converts our glm vec2's into a form appropriate for CGAL to operate on
            for (const auto& vertex : f_OptimizedVertices)
            {
                f_Polygon.push_back(Point_2(vertex.x, vertex.y));
            }

            //Run an initial decomposition, and check the results to see if all polygons obey the maxium upper bound of 8 vertices per polygon
            Polygon_List f_InitialDecomposition;
            CGAL::approx_convex_partition_2(f_Polygon.vertices_begin(), f_Polygon.vertices_end(), back_inserter(f_InitialDecomposition));

            bool f_NeedsFurtherDecomposition = true;
            // decompose any polygon with more than 8 vertices further
            while (f_NeedsFurtherDecomposition)
            {
                bool f_NeedsFurtherDecomposition = false;
                Polygon_List f_NextDecomposition;

                for (const auto& poly : f_InitialDecomposition)
                {
                    if (poly.size() <= 8) //verifies that all polygons are below 8 vertices if the initial pass has done the job, otherwise we go again baby
                    {
                        fp_OutputPolygons.push_back(poly);
                    }
                    else //reruns the algorithm as many times as needed to get a full decomposition
                    {
                        CGAL::approx_convex_partition_2(poly.vertices_begin(), poly.vertices_end(), back_inserter(f_NextDecomposition));
                        f_NeedsFurtherDecomposition = true;
                    }
                }

                //assert(CGAL::convex_partition_is_valid_2(polygon.vertices_begin(),
                //    polygon.vertices_end(),
                //    partition_polys.begin(),
                //    partition_polys.end()));

                if (f_NeedsFurtherDecomposition)
                {
                    f_InitialDecomposition = f_NextDecomposition;
                }
            }

            return true;

            //if (!CGAL::is_convex_2(fp_OutputPolygons.begin(), fp_OutputPolygons.end())) 
            //{
            //    LogManager::MainLogger().Warn("Failed to decompose polygon into convex pieces.", "CollisionPolygon2D");
            //    return false;
            //}
            //else 
            //{
            //    return true;
            //}
        }

        // Function to ensure vertices are wound counter-clockwise in Box2D
        vector<b2Vec2> AssertWindingOrderCCW(const vector<glm::vec2>& vertices)
        {
            vector<b2Vec2> box2dVertices;
            for (const auto& vertex : vertices)
            {
                box2dVertices.push_back(b2Vec2(vertex.x, vertex.y));
            }

            // Ensure counter-clockwise winding
            float area = 0.0f;
            for (size_t i = 0; i < box2dVertices.size(); ++i)
            {
                size_t j = (i + 1) % box2dVertices.size();
                area += box2dVertices[i].x * box2dVertices[j].y;
                area -= box2dVertices[i].y * box2dVertices[j].x;
            }

            // If area is negative, vertices are wound clockwise and need to be reversed
            if (area < 0.0f)
            {
                reverse(box2dVertices.begin(), box2dVertices.end());
            }

            return box2dVertices;
        }

        vector<b2Vec2> RemoveCollinearPoints(vector<b2Vec2>& fp_Vertices) 
        {
            vector<b2Vec2> result;

            for (size_t i = 0; i < fp_Vertices.size(); ++i)
            {
                size_t prev = (i + fp_Vertices.size() - 1) % fp_Vertices.size();
                size_t next = (i + 1) % fp_Vertices.size();

                b2Vec2 v1 = b2Vec2( (fp_Vertices[i].x - fp_Vertices[prev].x), (fp_Vertices[i].y - fp_Vertices[prev].y) );
                b2Vec2 v2 = b2Vec2( (fp_Vertices[next].x - fp_Vertices[i].x), (fp_Vertices[next].y - fp_Vertices[i].y) );

                if (b2Cross(v1, v2) != 0)
                {
                    result.push_back(fp_Vertices[i]);
                }
            }
            return result;
        }

    private:
        b2BodyId pm_Body2D = b2_nullBodyId;
        bool pm_IsValid = false;

        //vector<b2BodyId> pm_ListOfConstituentPolygons;
    };

} // >w<
