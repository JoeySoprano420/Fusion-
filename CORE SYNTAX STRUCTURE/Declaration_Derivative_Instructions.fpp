define player as Entity derives Movable, Damageable
define camera derives Follow(player)

init health = 100 : int
let score = 0 : uint
