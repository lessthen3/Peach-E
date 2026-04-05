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

#include "../PeachNode.h"

namespace PeachCore {

    struct Sprite2D
    {

        //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
    //Drawable GraphicsType; 
    //WARNING THIS NEEDS TO BE SWITCHED OFF FOR APPLE BUILDS SINCE TIM APPLE DECIDED NOT TO SUPPORT OPENGL ANYMORE UWU
    // OpenGLShaderProgram Shaders; //Contains multiple shaders relevant to drawing the object
    };

    struct Sprite3D //actually this is needed since itll have a 3d transform uwu
    {

    };

}