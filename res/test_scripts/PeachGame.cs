// PeachGame.dll
using Peach;

public class Player : PeachScript
{
    public override void OnStart() => Console.WriteLine("Start!");
    public override void OnConstantUpdate() => Console.WriteLine("Constant Update!");
    public override void OnUpdate() => Console.WriteLine("Update!");
    public override void OnExit() => Console.WriteLine("Exit!");
}
