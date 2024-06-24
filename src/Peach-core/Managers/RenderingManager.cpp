#include "RenderingManager.h"

void RenderingManager::Initialize(std::string p_RendererType, const std::string& title , int width , int height)
{
    if (hasBeenInitialized)
    {
        LogManager::Logger().Warn("RenderingManager has already been initialized, LogManager is only allowed to initialize once per run", "RenderingManager");
        return;
    }
    else
    {
        hasBeenInitialized = true;
    }
}

std::string RenderingManager::GetRendererType() const
{
    return pm_RendererType;
}