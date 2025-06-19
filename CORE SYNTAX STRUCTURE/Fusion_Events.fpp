on collision(player, enemy) {
    health -= 25
    if health <= 0 then trigger game_over()
}
