// PeachSharp.dll
namespace Peach
{
    public abstract class PeachScript
    {
        public abstract void OnStart();
        public abstract void OnUpdate();
        public abstract void OnConstantUpdate();
        public abstract void OnExit();
    }
}
