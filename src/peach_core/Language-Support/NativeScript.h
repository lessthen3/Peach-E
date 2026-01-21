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

struct NativeScript 
{
    virtual ~NativeScript() = default;
    virtual void Initialize() = 0;
    virtual void Update(double TimeSinceLastFrame) = 0;
    virtual void ConstantUpdate(double TimeSinceLastFrame) = 0;
    virtual void Shutdown() = 0;
};
