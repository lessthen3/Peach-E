#pragma once

<<<<<<< Updated upstream:src/Peach-core/include/Peach-Core2D/SceneTreeItems2D/Physics2D/CollisionSegment2D.h
#include "../../../Managers/LogManager.h"
=======
#include "../Managers/LogManager.h"
>>>>>>> Stashed changes:include/Peach-Core/2D/CollisionSegment2D.h

#include <vector>
#include <string>
#include <glm/glm.hpp>

using namespace std;

namespace PeachCore {

	class CollisionSegment2D
	{



	public:

		void AddPoint()
		{

		}

		void JoinSegment(CollisionSegment2D&& fp_Other) //joins two different segments into one segment
		{

		}


		string GetSegmentID()
			const
		{
			return pm_SegmentID;
		}

	private:

		//this is called everytime after the tilemap tiles are placed onto a cluster/island of tiles to create an outline for the collision
		void OptimizeLineSegment()
		{
			if(pm_ListOfPoints.size() == 0)
			{
				LogManager::MainLogger().Warn("Tried to optimize line segment when line segment has 0 points to optimize out at segment ID: " + pm_SegmentID, "CollisionSegment2D");
				return;
			}

			glm::vec2 f_LastPoint, f_CurrentPoint;
			int f_ParallelSegmentsVisitedCount = 0; //counts the amount of segments visited in a row that are straight

			//DOES THIS WHOLE THING NEED TO BE IN A WHILE LOOP ????? CAN I MODIFY THIS WHILE LOOPING IS THIS A GOOD LANGUAGE LIKE C#
			for (int index = 0; index < ((pm_ListOfPoints.size() - 1) - 1); index++) // - 1 again so that we reach the 0 based index - 1 so we dont go outta bounds
			{
				assert(index + 1 <= pm_ListOfPoints.size() - 1); //make sure index doesnt go out of bounds

				f_LastPoint = pm_ListOfPoints[index];
				f_CurrentPoint = pm_ListOfPoints[index + 1];

				if (((f_CurrentPoint.x != f_LastPoint.x) && (f_CurrentPoint.y != f_LastPoint.y)))
				{
					f_ParallelSegmentsVisitedCount = 1; //we've accumlated 1 segment if the first and last point are not parallel since this triggers only when f_LastPoint is an edge point of a tile chunk

				}
				//Need extra condition to stop the case when the two verts being compared are the start of a new direction on the tile chunk border, where if this wasn't here
				// f_ParallelSegmentsVisitedCount != 1, stops the algorithm from deleting visited corners of the tile chunk
				// index != 0 stops the algorithm from deleting the first point in the list, eg. a triangle
				else if (((f_CurrentPoint.x == f_LastPoint.x) || (f_CurrentPoint.y == f_LastPoint.y)) && f_ParallelSegmentsVisitedCount != 1 && index != 0)
				{
					//ERASE THE LAST POINT VISITED.
					pm_ListOfPoints.erase(pm_ListOfPoints.begin() + index); //I AM NOT SURE IF THIS IS ERASING THE RIGHT INDEX OH GOD
					f_ParallelSegmentsVisitedCount++;
				}

			}
		}



	private:
		vector<glm::vec2> pm_ListOfPoints;
		string pm_SegmentID;

	};

}