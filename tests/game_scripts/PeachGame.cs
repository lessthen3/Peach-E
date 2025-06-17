/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
using Peach;

public class Player : PeachScript
{
    public override void OnEnter() => Console.WriteLine("Start!");
    public override void OnConstantUpdate() => Console.WriteLine("Constant Update!");
    public override void OnUpdate() => Console.WriteLine("Update!");
    public override void OnExit() => Console.WriteLine("Exit!");
}
