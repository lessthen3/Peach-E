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
local Sprite2D = {}
Sprite2D.__index = Sprite2D

function Sprite2D.new(nodeID)
    return setmetatable({ id = nodeID, Position = {0, 0} }, Sprite2D)
end

function Sprite2D:move(dx, dy)
    PEACH_MoveNode2D(self.id, dx, dy)
end

function Sprite2D:set_visible(v)
    PEACH_SetNodeVisibility(self.id, v)
end
