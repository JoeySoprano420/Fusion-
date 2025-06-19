define enemy as Entity derives AIControlled
ai basic_enemy {
    think "Enemy scans for player"
}
init health = 50 : int
on update {
    print "Enemy is updating"
}
asset texture {
    load "enemy.png"
}
animation idle {
    animation "idle_loop"
}
