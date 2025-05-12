using System;
using System.Runtime.InteropServices;
using Peach;

public static class ScriptBridge
{
    [UnmanagedCallersOnly(EntryPoint = "CreateScriptInstance")]
    public static IntPtr CreateScriptInstance(string fp_ScriptName, string fp_TypeName)
    {
        var type = Type.GetType(fp_TypeName + ", " + fp_ScriptName);

        if (type == null)
            return IntPtr.Zero;

        var instance = Activator.CreateInstance(type) as PeachScript;

        if (instance == null)
            return IntPtr.Zero;

        GCHandle handle = GCHandle.Alloc(instance); // pin object

        return GCHandle.ToIntPtr(handle);
    }

    [UnmanagedCallersOnly(EntryPoint = "CallOnStart")]
    public static void CallOnStart(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        var script = (PeachScript)handle.Target;
        script.OnStart();
    }
    
    [UnmanagedCallersOnly(EntryPoint = "CallOnUpdate")]
    public static void CallOnUpdate(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        var script = (PeachScript)handle.Target;
        script.OnUpdate();
    }

    [UnmanagedCallersOnly(EntryPoint = "CallOnConstantUpdate")]
    public static void CallOnConstantUpdate(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        var script = (PeachScript)handle.Target;
        script.OnConstantUpdate();
    }

    [UnmanagedCallersOnly(EntryPoint = "CallOnExit")]
    public static void CallOnExit(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        var script = (PeachScript)handle.Target;
        script.OnExit();
    }

    [UnmanagedCallersOnly(EntryPoint = "ReleaseScript")]
    public static void ReleaseScript(IntPtr handlePtr)
    {
        var handle = GCHandle.FromIntPtr(handlePtr);
        handle.Free(); // release the object
    }
}
