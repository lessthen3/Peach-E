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
namespace Peach;

using System;
using System.Runtime.InteropServices;

public abstract class PeachScript
{
    public abstract void OnEnter();
    public abstract void OnUpdate();
    public abstract void OnConstantUpdate();
    public abstract void OnExit();
}

public static class Peach
{
    [DllImport("peach_api", CallingConvention = CallingConvention.Cdecl)]
    public static extern void Peach_Log(string msg);
}

