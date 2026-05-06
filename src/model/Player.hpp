
#ifndef SPIRELIKE_PLAYER_HPP
#define SPIRELIKE_PLAYER_HPP

struct Player {
    int maxHp = 70;
    int hp = 70;
    int block = 0;

    int strength = 0;
    int vulnerable = 0;
    int weak = 0;
};
#endif //SPIRELIKE_PLAYER_HPP