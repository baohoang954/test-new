#include <iostream>
#include <math.h>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include "graphics.h"
#include "defs.h"
#include "input.h"
#include "character.h"
#include "obstacle.h"
//cho bot vao char.cpp

int highScore = 0;

int main(int argc, char* argv[]) {
    // Load high score from file
    std::ifstream inFile("highscore.txt");
    if (!inFile.is_open()) {
        std::cerr << "Warning: Could not open highscore.txt for reading. Starting with highScore = 0.\n";
    } else {
        inFile >> highScore;
        inFile.close();
    }

    Graphics graphics;
    graphics.init();

    //textures
    ScrollingBackground background;
    background.setTexture(graphics.loadTexture(BACKGROUND_IMG));
    SDL_Texture* menuTexture = graphics.loadTexture("img/menu_screen.png");
    SDL_Texture* gameOverTexture = graphics.loadTexture("img/game_over.png");
    if (!menuTexture || !gameOverTexture) graphics.logErrorAndExit("Failed to load menu or game over texture", IMG_GetError());

    //music
    Mix_Music* menuMusic = Mix_LoadMUS("audio/menu_music.mp3");
    Mix_Music* gameMusic = Mix_LoadMUS("audio/game_music.mp3");
    Mix_Music* gameOverMusic = Mix_LoadMUS("audio/game_over_music.mp3");
    if (!menuMusic || !gameMusic || !gameOverMusic) {
        SDL_Log("Failed to load music: %s", Mix_GetError());
    }

    //character
    Character player;
    SDL_Texture* playerTexture = graphics.loadTexture("img/player_anims.png");
    player.setTexture(playerTexture, 32, 64);
    player.setPosition(100, SCREEN_HEIGHT - player.getHeight() - 20);

    Input input;

    GameState state = MENU;
    bool quit = false;
    bool isMoving = false;
    bool faceRight = true;
    bool isJumping = false;
    int jumpHeight = 0;
    const int jumpMax = 100;
    Uint32 lastJumpTime = 0;
    Uint32 lastJumpCooldownTime = 0; //jump cooldown
    const Uint32 jumpCooldown = 1000; //chinh cooldown (1 giay atm)
    int speed = 0;
    const int maxSpeed = 100;
    Uint32 lastSpeedDrainTime = 0;
    const Uint32 speedDrainInterval = 500;
    std::vector<Obstacle> obstacles;
    int obstacleSpawnTimer = 0;
    int nextObstacleInterval = 0; // khoang cach vat can ()
    Uint32 runStartTime = 0;
    bool hasStartedRunning = false; // kiem tra nv chay chua
    const Uint32 runDelay = 3000; // delay dau game de chuan bi(3 giay)
    const int obstacleBaseSpeed = 3;
    int cameraOffset = 0;
    Uint32 gameStartTime = 0;
    const Uint32 gracePeriod = 2000; //gioi han tgian(dung sau)
    const int fixedScreenX = 100; // vi tri nv

    if (menuMusic && !input.isMuted()) Mix_PlayMusic(menuMusic, -1);

    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();

        switch (state) {
            case MENU: {
                graphics.prepareScene();
                int menuWidth, menuHeight;
                SDL_QueryTexture(menuTexture, NULL, NULL, &menuWidth, &menuHeight);
                graphics.renderTexture(menuTexture, (SCREEN_WIDTH - menuWidth) / 2, (SCREEN_HEIGHT - menuHeight) / 2);

                // CHINH SUA CHU

                TTF_Init();
                TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
                SDL_Color blue = {0, 0, 255, 255}; // menu text(blue)
                SDL_Color black = {0, 0, 0, 255}; // vien text

                //chinh sua chu high score
                SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, ("High Score: " + std::to_string(highScore)).c_str(), black);
                SDL_Surface* highScoreSurfaceColor = TTF_RenderText_Solid(font, ("High Score: " + std::to_string(highScore)).c_str(), blue);
                SDL_Texture* highScoreText = SDL_CreateTextureFromSurface(graphics.renderer, highScoreSurface);
                SDL_Texture* highScoreTextColor = SDL_CreateTextureFromSurface(graphics.renderer, highScoreSurfaceColor);
                SDL_Rect highScoreRect = {(SCREEN_WIDTH - 200) / 2, (SCREEN_HEIGHT - menuHeight) / 2 + 10, 200, 24};
                // vien ngoai chu high score
                SDL_Rect outlineRect = highScoreRect;
                outlineRect.x -= 1;
                SDL_RenderCopy(graphics.renderer, highScoreText, NULL, &outlineRect);
                outlineRect.x += 2;
                SDL_RenderCopy(graphics.renderer, highScoreText, NULL, &outlineRect);
                outlineRect.x -= 1; outlineRect.y -= 1;
                SDL_RenderCopy(graphics.renderer, highScoreText, NULL, &outlineRect);
                outlineRect.y += 2;
                SDL_RenderCopy(graphics.renderer, highScoreText, NULL, &outlineRect);

                SDL_RenderCopy(graphics.renderer, highScoreTextColor, NULL, &highScoreRect);


                SDL_Surface* startSurface = TTF_RenderText_Solid(font, "Press Enter to Start", black);
                SDL_Surface* startSurfaceColor = TTF_RenderText_Solid(font, "Press Enter to Start", blue);
                SDL_Texture* startText = SDL_CreateTextureFromSurface(graphics.renderer, startSurface);
                SDL_Texture* startTextColor = SDL_CreateTextureFromSurface(graphics.renderer, startSurfaceColor);
                SDL_Rect startRect = {(SCREEN_WIDTH - 200) / 2, (SCREEN_HEIGHT + menuHeight) / 2 - 50, 200, 24};
                // vien den
                outlineRect = startRect;
                outlineRect.x -= 1;
                SDL_RenderCopy(graphics.renderer, startText, NULL, &outlineRect);
                outlineRect.x += 2;
                SDL_RenderCopy(graphics.renderer, startText, NULL, &outlineRect);
                outlineRect.x -= 1; outlineRect.y -= 1;
                SDL_RenderCopy(graphics.renderer, startText, NULL, &outlineRect);
                outlineRect.y += 2;
                SDL_RenderCopy(graphics.renderer, startText, NULL, &outlineRect);

                SDL_RenderCopy(graphics.renderer, startTextColor, NULL, &startRect);


                SDL_Surface* muteSurface = TTF_RenderText_Solid(font, "Press M to Mute", black);
                SDL_Surface* muteSurfaceColor = TTF_RenderText_Solid(font, "Press M to Mute", blue);
                SDL_Texture* muteText = SDL_CreateTextureFromSurface(graphics.renderer, muteSurface);
                SDL_Texture* muteTextColor = SDL_CreateTextureFromSurface(graphics.renderer, muteSurfaceColor);
                SDL_Rect muteRect = {(SCREEN_WIDTH - 200) / 2, (SCREEN_HEIGHT + menuHeight) / 2 - 20, 200, 24};

                outlineRect = muteRect;
                outlineRect.x -= 1;
                SDL_RenderCopy(graphics.renderer, muteText, NULL, &outlineRect);
                outlineRect.x += 2;
                SDL_RenderCopy(graphics.renderer, muteText, NULL, &outlineRect);
                outlineRect.x -= 1; outlineRect.y -= 1;
                SDL_RenderCopy(graphics.renderer, muteText, NULL, &outlineRect);
                outlineRect.y += 2;
                SDL_RenderCopy(graphics.renderer, muteText, NULL, &outlineRect);

                SDL_RenderCopy(graphics.renderer, muteTextColor, NULL, &muteRect);

                SDL_FreeSurface(highScoreSurface);
                SDL_FreeSurface(highScoreSurfaceColor);
                SDL_FreeSurface(startSurface);
                SDL_FreeSurface(startSurfaceColor);
                SDL_FreeSurface(muteSurface);
                SDL_FreeSurface(muteSurfaceColor);
                SDL_DestroyTexture(highScoreText);
                SDL_DestroyTexture(highScoreTextColor);
                SDL_DestroyTexture(startText);
                SDL_DestroyTexture(startTextColor);
                SDL_DestroyTexture(muteText);
                SDL_DestroyTexture(muteTextColor);
                TTF_CloseFont(font);
                TTF_Quit();

                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) quit = true;
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                            state = PLAYING;
                            gameStartTime = currentTime;
                            SDL_Log("Transition to PLAYING state");
                            if (gameMusic && !input.isMuted()) {
                                Mix_HaltMusic();
                                Mix_PlayMusic(gameMusic, -1);
                            }
                        }
                        if (e.key.keysym.scancode == SDL_SCANCODE_M) input.toggleMute();
                    }
                }
                break;
            }

            case PLAYING: {
                input.handleInput(quit, isMoving, faceRight, player, currentTime, isJumping, jumpHeight, jumpMax, lastJumpTime, lastJumpCooldownTime, jumpCooldown, state, speed, maxSpeed, obstacles, cameraOffset);
                player.update(isMoving, currentTime, speed);

                if (isJumping) {
                    if (currentTime - lastJumpTime < 500) {
                        jumpHeight = jumpMax * sin((currentTime - lastJumpTime) * 3.14 / 500);
                        player.setPosition(player.getX(), SCREEN_HEIGHT - player.getHeight() - 20 - jumpHeight);
                    } else {
                        isJumping = false;
                        jumpHeight = 0;
                        lastJumpCooldownTime = currentTime; //s tart cooldown when jump ends
                        player.setPosition(player.getX(), SCREEN_HEIGHT - player.getHeight() - 20);
                        SDL_Log("Jump ended, cooldown started at %u", lastJumpCooldownTime);
                    }
                }

                graphics.prepareScene();
                //calculate potential new position
                float moveSpeed = (speed > 0) ? (speed / 14.0f) : 0; //0 to 7.14 pixels per frame
                int newX = player.getX() + static_cast<int>(moveSpeed);

                //cham vat can
                bool blocked = false;
                for (const auto& obstacle : obstacles) {
                    SDL_Rect obsRect = {obstacle.x - cameraOffset, obstacle.y, obstacle.width, obstacle.height};
                    SDL_Rect playerRect = {fixedScreenX, player.getY(), player.getWidth(), player.getHeight()};
                    if (SDL_HasIntersection(&playerRect, &obsRect) && !isJumping) {
                        blocked = true;
                        speed = 0; //game over,co the chinh de vat can chi giam toc do
                        SDL_Log("Player blocked by obstacle at x=%d", obstacle.x);
                        break;
                    }
                }


                if (!blocked) {
                    player.setPosition(newX, player.getY());
                }

                //code camera chay the nv,nv dung yen
                cameraOffset = player.getX() - fixedScreenX;
                background.scroll(static_cast<int>(moveSpeed));
                graphics.render(background);
                SDL_Log("Move speed: %.2f, Speed: %d, Camera offset: %d, Player X: %d", moveSpeed, speed, cameraOffset, player.getX());

                //ktra chay
                if (speed > 0 && !hasStartedRunning) {
                    runStartTime = currentTime;
                    hasStartedRunning = true;
                    SDL_Log("Player started running at time: %u", runStartTime);
                } else if (speed <= 0) {
                    hasStartedRunning = false; // Reset if speed drops to 0
                }

                // 3 giay sinh obs
                if (hasStartedRunning && (currentTime - runStartTime >= runDelay) && speed > 0 && !blocked) {
                    obstacleSpawnTimer += 16;
                    if (obstacleSpawnTimer >= nextObstacleInterval) {
                        obstacles.emplace_back(SCREEN_WIDTH + cameraOffset, SCREEN_HEIGHT - 64);
                        // random obs (1000 - 3000 ms)
                        srand(currentTime);
                        nextObstacleInterval = 1000 + (rand() % 2000);
                        obstacleSpawnTimer = 0;
                        SDL_Log("Obstacle spawned at x=%d, Next interval=%dms", obstacles.back().x, nextObstacleInterval);
                    }
                }

                for (auto it = obstacles.begin(); it != obstacles.end();) {
                    if (speed > 0) it->x -= obstacleBaseSpeed + (moveSpeed > 3 ? 3 : 0);
                    if (it->x - cameraOffset + it->width < 0) {
                        it = obstacles.erase(it);
                    } else {
                        SDL_Rect obsRect = {it->x - cameraOffset, it->y, it->width, it->height};
                        SDL_SetRenderDrawColor(graphics.renderer, 255, 255, 0, 255);
                        SDL_RenderFillRect(graphics.renderer, &obsRect);
                        ++it;
                    }
                }

                //nv sinh goc trai
                SDL_Rect destRect = {fixedScreenX - player.getWidth() / 2, player.getY(), player.getWidth(), player.getHeight()};
                SDL_RenderCopyEx(graphics.renderer, player.getTexture(), player.getSrcRect(), &destRect, 0, NULL, SDL_FLIP_NONE);
                SDL_Scancode currentKey = input.getCurrentKey();
                SDL_Log("Rendering key: %s (%d)", SDL_GetScancodeName(currentKey), currentKey);
                player.renderKey(graphics.renderer, currentKey);
                player.renderSpeedBar(graphics.renderer, speed, maxSpeed);

                //render score in top right corner with black outline
                TTF_Init();
                TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
                SDL_Color green = {0, 255, 0, 255}; // Green color
                SDL_Color black = {0, 0, 0, 255}; // Black for outline
                SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, ("Score: " + std::to_string(player.getX())).c_str(), black);
                SDL_Surface* scoreSurfaceColor = TTF_RenderText_Solid(font, ("Score: " + std::to_string(player.getX())).c_str(), green);
                SDL_Texture* scoreText = SDL_CreateTextureFromSurface(graphics.renderer, scoreSurface);
                SDL_Texture* scoreTextColor = SDL_CreateTextureFromSurface(graphics.renderer, scoreSurfaceColor);
                SDL_Rect scoreRect = {SCREEN_WIDTH - 150, 10, 140, 24};
                //outline
                SDL_Rect outlineRect = scoreRect;
                outlineRect.x -= 1;
                SDL_RenderCopy(graphics.renderer, scoreText, NULL, &outlineRect);
                outlineRect.x += 2;
                SDL_RenderCopy(graphics.renderer, scoreText, NULL, &outlineRect);
                outlineRect.x -= 1; outlineRect.y -= 1;
                SDL_RenderCopy(graphics.renderer, scoreText, NULL, &outlineRect);
                outlineRect.y += 2;
                SDL_RenderCopy(graphics.renderer, scoreText, NULL, &outlineRect);

                SDL_RenderCopy(graphics.renderer, scoreTextColor, NULL, &scoreRect);
                SDL_FreeSurface(scoreSurface);
                SDL_FreeSurface(scoreSurfaceColor);
                SDL_DestroyTexture(scoreText);
                SDL_DestroyTexture(scoreTextColor);
                TTF_CloseFont(font);
                TTF_Quit();

                //reset character if too far right
                if (player.getX() > cameraOffset + SCREEN_WIDTH + 200) {
                    player.setPosition(cameraOffset + 100, SCREEN_HEIGHT - player.getHeight() - 20);
                    SDL_Log("Character reset: New X = %d", player.getX());
                }

                //game over (het tgian vs speed off)
                if (currentTime - gameStartTime > gracePeriod && speed <= 0 && !isMoving) {
                    state = GAME_OVER;
                    SDL_Log("Game over: Speed = %d, isMoving = %d", speed, isMoving);
                    if (gameOverMusic && !input.isMuted()) {
                        Mix_HaltMusic();
                        Mix_PlayMusic(gameOverMusic, 1);
                    }
                    if (player.getX() > highScore) {
                        highScore = static_cast<int>(player.getX());
                        std::ofstream outFile("highscore.txt");
                        if (outFile.is_open()) {
                            outFile << highScore;
                            outFile.close();
                        }
                    }
                }

                //speed bar drain
                if (currentTime - gameStartTime > gracePeriod && currentTime - lastSpeedDrainTime >= speedDrainInterval) {
                    if (!isMoving || blocked) {
                        speed = std::max(0, speed - 1);
                        SDL_Log("Speed drain: Speed = %d", speed);
                    }
                    lastSpeedDrainTime = currentTime;
                }
                break;
            }

            case GAME_OVER: {
                graphics.prepareScene();
                //chinh anh end screen
                int gameOverWidth, gameOverHeight;
                SDL_QueryTexture(gameOverTexture, NULL, NULL, &gameOverWidth, &gameOverHeight);
                graphics.renderTexture(gameOverTexture, (SCREEN_WIDTH - gameOverWidth) / 2 - 100, (SCREEN_HEIGHT - gameOverHeight) / 2 - 70);

                TTF_Init();
                TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
                SDL_Color white = {255, 255, 255, 255};

                //"GAME OVER"
                SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "GAME OVER", white);
                SDL_Texture* gameOverText = SDL_CreateTextureFromSurface(graphics.renderer, gameOverSurface);
                int gameOverTextWidth, gameOverTextHeight;
                SDL_QueryTexture(gameOverText, NULL, NULL, &gameOverTextWidth, &gameOverTextHeight);
                SDL_Rect gameOverRect = {(SCREEN_WIDTH - gameOverTextWidth) / 2 - 100, (SCREEN_HEIGHT - gameOverTextHeight) / 2 - 100, gameOverTextWidth, gameOverTextHeight};
                SDL_RenderCopy(graphics.renderer, gameOverText, NULL, &gameOverRect);

                SDL_Surface* restartSurface = TTF_RenderText_Solid(font, "Press Enter to Restart", white);
                SDL_Texture* restartText = SDL_CreateTextureFromSurface(graphics.renderer, restartSurface);
                SDL_Rect restartRect = {(SCREEN_WIDTH - 200) / 2 - 100, (SCREEN_HEIGHT - 24) / 2 + 50 - 70, 200, 24};
                SDL_RenderCopy(graphics.renderer, restartText, NULL, &restartRect);

                SDL_Surface* quitSurface = TTF_RenderText_Solid(font, "Press Escape to Quit", white);
                SDL_Texture* quitText = SDL_CreateTextureFromSurface(graphics.renderer, quitSurface);
                SDL_Rect quitRect = {(SCREEN_WIDTH - 200) / 2 - 100, (SCREEN_HEIGHT - 24) / 2 + 100 - 70, 200, 24};
                SDL_RenderCopy(graphics.renderer, quitText, NULL, &quitRect);

                SDL_FreeSurface(gameOverSurface);
                SDL_FreeSurface(restartSurface);
                SDL_FreeSurface(quitSurface);
                SDL_DestroyTexture(gameOverText);
                SDL_DestroyTexture(restartText);
                SDL_DestroyTexture(quitText);
                TTF_CloseFont(font);
                TTF_Quit();

                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) quit = true;
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                            state = PLAYING;
                            player.setPosition(100, SCREEN_HEIGHT - player.getHeight() - 20);
                            speed = 0;
                            obstacles.clear();
                            cameraOffset = 0;
                            background.scrollingOffset = 0;
                            gameStartTime = currentTime;
                            hasStartedRunning = false; //restart
                            obstacleSpawnTimer = 0;
                            nextObstacleInterval = 0;
                            SDL_Log("Restarting game");
                            if (gameMusic && !input.isMuted()) {
                                Mix_HaltMusic();
                                Mix_PlayMusic(gameMusic, -1);
                            }
                        }
                        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) quit = true;
                        if (e.key.keysym.scancode == SDL_SCANCODE_M) input.toggleMute();
                    }
                }
                break;
            }
        }

        graphics.presentScene();
        SDL_Delay(16);
    }

    //save high score
    std::ofstream outFile("highscore.txt");
    if (outFile.is_open()) {
        outFile << highScore;
        outFile.close();
    }

    SDL_DestroyTexture(background.texture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(gameOverTexture);
    if (menuMusic) Mix_FreeMusic(menuMusic);
    if (gameMusic) Mix_FreeMusic(gameMusic);
    if (gameOverMusic) Mix_FreeMusic(gameOverMusic);
    graphics.quit();

    return 0;
}
