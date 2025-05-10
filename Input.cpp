#include "input.h"
#include "defs.h"
#include <SDL_mixer.h>
#include <cstdlib>
#include <ctime>

void Input::handleInput(bool& quit, bool& isMoving, bool& faceRight, Character& player, Uint32 currentTime, bool& isJumping, int& jumpHeight, int jumpMax, Uint32& lastJumpTime, Uint32& lastJumpCooldownTime, const Uint32 jumpCooldown, GameState& state, int& speed, const int& maxSpeed, std::vector<Obstacle>& obstacles, int& cameraOffset) {
    SDL_Event e;
    isMoving = false;
    static Uint32 lastKeyPressTime = 0;
    const Uint32 keyCooldown = 200; //200ms cooldown prvent spam
    const Uint32 retryThreshold = 100; //time before new key prompt after incorrect press

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
            SDL_Log("Quit event received");
        }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.scancode == SDL_SCANCODE_SPACE && !isJumping && (currentTime - lastJumpCooldownTime >= jumpCooldown)) {
                isJumping = true;
                lastJumpTime = currentTime;
                SDL_Log("Space pressed: Jumping");
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_M) {
                toggleMute();
                SDL_Log("M pressed: Toggle mute");
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_RETURN && state == GAME_OVER) {
                state = PLAYING;
                player.setPosition(100, SCREEN_HEIGHT - player.getHeight() - 20);
                speed = 0;
                obstacles.clear();
                cameraOffset = 0;
                lastKeyPressTime = currentTime;
                srand(currentTime);
                SDL_Scancode keys[] = {SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D};
                currentKey = keys[rand() % 4];
                lastKeyTime = currentTime;
                SDL_Log("Enter pressed: Game restarted, initial key = %s (%d)", SDL_GetScancodeName(currentKey), currentKey);
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                quit = true;
                SDL_Log("Escape pressed: Quitting");
            }
            if (state == PLAYING && currentTime - lastKeyPressTime >= keyCooldown &&
                (e.key.keysym.scancode == SDL_SCANCODE_A || e.key.keysym.scancode == SDL_SCANCODE_W ||
                 e.key.keysym.scancode == SDL_SCANCODE_S || e.key.keysym.scancode == SDL_SCANCODE_D)) {
                const char* keyName = SDL_GetScancodeName(e.key.keysym.scancode);
                const char* expectedKeyName = SDL_GetScancodeName(currentKey);
                if (e.key.keysym.scancode == currentKey) {
                    speed = std::min(maxSpeed, speed + 10);
                    isMoving = true;
                    lastKeyPressTime = currentTime;
                    //newkey
                    srand(currentTime);
                    SDL_Scancode keys[] = {SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D};
                    currentKey = keys[rand() % 4];
                    lastKeyTime = currentTime;
                    SDL_Log("Correct key (%s) pressed: Speed = %d, New key = %s (%d)", keyName, speed, SDL_GetScancodeName(currentKey), currentKey);
                } else {
                    speed = std::max(0, speed - 20);
                    lastKeyPressTime = currentTime;
                    //bam sai->neu key
                    if (currentTime - lastKeyTime > retryThreshold) {
                        srand(currentTime);
                        SDL_Scancode keys[] = {SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D};
                        currentKey = keys[rand() % 4];
                        lastKeyTime = currentTime;
                        SDL_Log("Wrong key (%s) pressed, expected %s: Speed = %d, New key = %s (%d)", keyName, expectedKeyName, speed, SDL_GetScancodeName(currentKey), currentKey);
                    } else {
                        SDL_Log("Wrong key (%s) pressed, expected %s: Speed = %d, Keeping key", keyName, expectedKeyName, speed);
                    }
                }
            }
        }
    }

    if (state == PLAYING) {
        if (isMoving) faceRight = true;
        player.setFlip(faceRight);
    }
}

void Input::toggleMute() {
    muted = !muted;
    if (muted) Mix_PauseMusic();
    else Mix_ResumeMusic();
}
