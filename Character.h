#ifndef _CHARACTER__H
#define _CHARACTER__H

#include <SDL.h>

class Character {
private:
    SDL_Texture* texture;
    int x, y;
    int currentFrame = 0;
    int frameWidth, frameHeight;
    int idleFrames = 3;
    int walkFrames = 8;
    bool isMoving = false;
    Uint32 lastFrameTime = 0;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    int speed = 0; // store current speed
    SDL_Rect srcRect;

public:
    void setTexture(SDL_Texture* _texture, int _frameWidth, int _frameHeight);
    void update(bool moving, Uint32 currentTime, int speed);
    void render(SDL_Renderer* renderer);
    void setPosition(int _x, int _y) { x = _x; y = _y; }
    void setFlip(bool faceRight) { flip = faceRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL; }
    int getWidth() const { return frameWidth; }
    int getHeight() const { return frameHeight; }
    int getX() const { return x; }
    int getY() const { return y; }
    void renderKey(SDL_Renderer* renderer, SDL_Scancode key);
    void renderSpeedBar(SDL_Renderer* renderer, int speed, int maxSpeed);
    SDL_Texture* getTexture() const;
    SDL_Rect* getSrcRect();
};

#endif // _CHARACTER__H
