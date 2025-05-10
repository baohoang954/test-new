#include "character.h"
#include <math.h>
#include <SDL.h>
#include <SDL_ttf.h>

void Character::setTexture(SDL_Texture* _texture, int _frameWidth, int _frameHeight)
{
    texture = _texture;
    frameWidth = _frameWidth;
    frameHeight = _frameHeight / 2;
    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    if (_frameHeight * 2 != texH) { SDL_Log("Warning: Provided frame height doesn't match actual sprite sheet height"); }
    srcRect = {0, 0, frameWidth, frameHeight};
}

void Character::update(bool moving, Uint32 currentTime, int speed) {
    isMoving = moving;
    this->speed = speed; //update speed

    //scale frametime based on speed (500ms speed=0, 40ms speed=100)
    int frameTime = (speed > 0) ? std::max(40, 500 - (speed * 46 / 10)) : 200; // 40ms to 500ms

    if (currentTime - lastFrameTime > frameTime) {
        if (speed > 0) {
            currentFrame = (currentFrame + 1) % walkFrames;
            srcRect = {currentFrame * frameWidth, frameHeight, frameWidth, frameHeight};
        } else {
            currentFrame = (currentFrame + 1) % idleFrames;
            srcRect = {currentFrame * frameWidth, 0, frameWidth, frameHeight};
        }
        lastFrameTime = currentTime;
    }
}

void Character::render(SDL_Renderer* renderer) {
    SDL_Rect destRect = {x, y, frameWidth, frameHeight};
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, NULL, flip);
}

SDL_Texture* Character::getTexture() const {
    return texture;
}

SDL_Rect* Character::getSrcRect() {
    return &srcRect;
}

void Character::renderKey(SDL_Renderer* renderer, SDL_Scancode key) {
    const char* keyText;
    switch (key) {
        case SDL_SCANCODE_A: keyText = "A"; break;
        case SDL_SCANCODE_W: keyText = "W"; break;
        case SDL_SCANCODE_S: keyText = "S"; break;
        case SDL_SCANCODE_D: keyText = "D"; break;
        default: keyText = "?"; break;
    }
    SDL_Log("renderKey: Displaying %s (%d)", keyText, key);

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) SDL_Log("Font load error: %s", TTF_GetError());

    SDL_Color yellow = {157, 0, 255, 0}; // Yellow color for key prompt
    SDL_Surface* surface = TTF_RenderText_Solid(font, keyText, yellow);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surface);

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    const int fixedScreenX = 100; // Fixed position on the left
    SDL_Rect textRect = {fixedScreenX + frameWidth / 2 - textWidth / 2, y - textHeight - 10, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    TTF_Quit();
}

void Character::renderSpeedBar(SDL_Renderer* renderer, int speed, int maxSpeed) {
    const int fixedScreenX = 100;
    int barWidth = 50;
    int barHeight = 10;
    int barX = fixedScreenX + frameWidth / 2 - barWidth / 2;
    int barY = y + frameHeight + 5;

    SDL_Rect backgroundRect = {barX, barY, barWidth, barHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &backgroundRect);

    SDL_Rect fillRect = {barX, barY, (speed * barWidth) / maxSpeed, barHeight};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &fillRect);
}
