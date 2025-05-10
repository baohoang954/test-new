#ifndef _INPUT__H
#define _INPUT__H

#include <SDL.h>
#include "character.h"
#include "obstacle.h"
#include "defs.h"
#include <vector>

class Input {
private:
    Uint32 lastKeyTime = 0;
    SDL_Scancode lastKey = SDL_SCANCODE_UNKNOWN;
    const Uint32 speedBoostThreshold = 100;
    SDL_Scancode currentKey = SDL_SCANCODE_A;
    bool muted = false;

public:
    void handleInput(bool& quit, bool& isMoving, bool& faceRight, Character& player, Uint32 currentTime, bool& isJumping, int& jumpHeight, int jumpMax, Uint32& lastJumpTime, Uint32& lastJumpCooldownTime, const Uint32 jumpCooldown, GameState& state, int& speed, const int& maxSpeed, std::vector<Obstacle>& obstacles, int& cameraOffset);
    SDL_Scancode getCurrentKey() const { return currentKey; }
    bool isMuted() const { return muted; }
    void toggleMute();
};

#endif
