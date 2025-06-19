instruction move_right(speed: float) {
    player.x += speed
    return position(player.x, player.y)
}
