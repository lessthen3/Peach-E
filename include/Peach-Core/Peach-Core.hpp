#pragma once

//////////////////////////////////////////////
// Language Support
//////////////////////////////////////////////

#include "Language-Support/DotnetRuntime.h"
#include "Language-Support/LuaScriptRuntime.h"
#include "Language-Support/PythonScriptComponent.h"
#include "Language-Support/PythonScriptRuntime.h"

//////////////////////////////////////////////
// Managers
//////////////////////////////////////////////

#include "Managers/AudioManager.h"
#include "Managers/LogManager.h"
#include "Managers/RenderingManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/PhysicsManager2D.h"
#include "Managers/NetworkManager.h"

//////////////////////////////////////////////
// Graphics2D
//////////////////////////////////////////////

#include "Scene-Items/2D/PeachCamera2D.h"
#include "Scene-Items/2D/PeachTexture2D.h"
#include "Scene-Items/2D/DirectionalLight2D.h"
#include "Scene-Items/2D/PointLight2D.h"

//////////////////////////////////////////////
// Physics2D
//////////////////////////////////////////////

#include "Scene-Items/2D/Joint2D.h"
#include "Scene-Items/2D/RayCast2D.h"

#include "Scene-Items/2D/CollisionPolygon2D.h"
#include "Scene-Items/2D/CollisionShape2D.h"
#include "Scene-Items/2D/CollisionSegment2D.h"
#include "Scene-Items/2D/Sensor2D.h"

//////////////////////////////////////////////
// Geometry2D
//////////////////////////////////////////////

#include "Scene-Items/2D/Curve2D.h"
#include "Scene-Items/2D/Grid2D.h"
#include "Scene-Items/2D/Path2D.h"
#include "Scene-Items/2D/PathFinder2D.h"

//////////////////////////////////////////////
// 3D
//////////////////////////////////////////////

#include "Scene-Items/3D/Camera3D.h"
#include "Scene-Items/3D/DirectionalLight3D.h"
#include "Scene-Items/3D/PointLight3D.h"
#include "Scene-Items/3D/SpotLight3D.h"

//////////////////////////////////////////////
// General
//////////////////////////////////////////////

#include "Scene-Items/PeachNode.h"
#include "Scene-Items/Scene.h"

//////////////////////////////////////////////
// Utils
//////////////////////////////////////////////

#include "Utils/PeachConsole.h"
#include "Utils/InputHandler.h"
#include "Utils/EventBoard.h"
#include "Utils/Serializer.h"
#include "Utils/NoiseGenerator.h"
#include "Utils/DebugUtils.h"


//////////////////////////////////////////////
// Rendering
//////////////////////////////////////////////

#include "Rendering/OpenGLShaderProgram.h"

//////////////////////////////////////////////
// UI
//////////////////////////////////////////////

#include "Scene-Items/UI/PeachUserInterfaceNode.h"

//////////////////////////////////////////////
// Import Header For Peach Engine's Core Library
//////////////////////////////////////////////