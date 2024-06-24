#pragma once
// Plugin.h
#pragma once

class Plugin {
public:
    virtual ~Plugin() = default;
    virtual void Initialize() = 0;
    virtual void Update(float TimeSinceLastFrame) = 0;
    virtual void ConstantUpdate(float TimeSinceLastFrame) = 0;
    virtual void Shutdown() = 0;
};
