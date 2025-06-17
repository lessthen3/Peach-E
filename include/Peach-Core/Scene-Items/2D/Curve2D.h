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



namespace PeachCore { //add graph viewer where the list of points from curve2d is graphed, dots connected and smoothed best i can, in a small window in editor to show path

	struct Curve2D 
	{

	public:
		void AddGraphParametricPoint(float fp_T); //Add function appends 1 point to the end of the curve2D list
		void AddParameterizedCircleEquationPoint(float fp_T);
		//void AddPolynomialTermPoint

		void AddGraphDirect(float fp_T1, float fp_T2); //just fucking do whatever u want, this method will go brazy, eg (e^x*Sin(sqrt(x)), x^x)
	};

}
