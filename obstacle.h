#ifndef _OBSTACLE__H
#define _OBSTACLE__H

struct Obstacle {
    int x, y;
    int width = 16, height = 64;
    Obstacle(int _x, int _y) : x(_x), y(_y) {}
};

#endif // _OBSTACLE__H
