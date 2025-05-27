#pragma once

//#include "" SHOULD INCLUDE THE ENTIRE ENGINE HERE, AND GIVE THIS OUT, IM COMMENTING IT OUT SO WE CAN COMPILE WITHOUT ERRORS

class Plugin 
{
public:
    virtual ~Plugin() = default;
    virtual void Initialize() = 0;
    virtual void Update(float TimeSinceLastFrame) = 0;
    virtual void ConstantUpdate(float TimeSinceLastFrame) = 0;
    virtual void Shutdown() = 0;
};
