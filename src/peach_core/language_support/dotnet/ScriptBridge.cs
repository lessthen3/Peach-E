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
namespace PeachScriptCore;

using System;
using System.Runtime.InteropServices;

public static class ScriptBridge
{
    [UnmanagedCallersOnly(EntryPoint = "CreateScriptInstance")]
    public static IntPtr 
        CreateScriptInstance(IntPtr fp_ScriptName, IntPtr fp_TypeName)
    {
        if (fp_ScriptName == IntPtr.Zero || fp_TypeName == IntPtr.Zero)
        {
            return IntPtr.Zero;
        }
        
        string f_ScriptName = Marshal.PtrToStringUTF8(fp_ScriptName);
        string f_TypeName = Marshal.PtrToStringUTF8(fp_TypeName);

        if (string.IsNullOrEmpty(f_ScriptName) || string.IsNullOrEmpty(f_TypeName))
        {
            return IntPtr.Zero;
        }

        var f_ScriptType = Type.GetType(f_TypeName + ", " + f_ScriptName);

        if (f_ScriptType == null)
        {
            return IntPtr.Zero;
        }

        if (Activator.CreateInstance(f_ScriptType) is not PeachScript f_ScriptInstance)
        {
            return IntPtr.Zero;
        }
        
        GCHandle f_ScriptInstanceGCHandle = GCHandle.Alloc(f_ScriptInstance); // pin object OwO

        return GCHandle.ToIntPtr(f_ScriptInstanceGCHandle);
    }

    [UnmanagedCallersOnly(EntryPoint = "CallOnEnter")]
    public static void 
        CallOnEnter(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        var script = (PeachScript)handle.Target;
        script.OnEnter();
    }

    [UnmanagedCallersOnly(EntryPoint = "CallOnUpdate")]
    public static void 
        CallOnUpdate(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        var script = (PeachScript)handle.Target;
        script.OnUpdate();
    }

    [UnmanagedCallersOnly(EntryPoint = "CallOnConstantUpdate")]
    public static void 
        CallOnConstantUpdate(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        var script = (PeachScript)handle.Target;
        script.OnConstantUpdate();
    }

    [UnmanagedCallersOnly(EntryPoint = "CallOnExit")]
    public static void
        CallOnExit(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        var script = (PeachScript)handle.Target;
        script.OnExit();
    }

    [UnmanagedCallersOnly(EntryPoint = "ReleaseScript")]
    public static void 
        ReleaseScript(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        handle.Free(); // release the object
    }
}
