define player as Entity derives Controllable, Renderable
define enemy as Entity derives AIControlled

init health = 100 : int
let speed = 0.25 : float

instruction move_player {
    if input.pressed("right") then player.x += speed
    if input.pressed("left") then  player.x -= speed
}

on update {
    call move_player
    if distance(player, enemy) < 5 then trigger collision(player, enemy)
}

on collision(player, enemy) {
    health -= 10
    if health <= 0 then trigger game_over()
}
