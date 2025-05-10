#ifndef _DEFS__H
#define _DEFS__H

const int SCREEN_WIDTH = 650;
const int SCREEN_HEIGHT = 310;
constexpr const char* WINDOW_TITLE = "Speed Runner";
constexpr const char* BACKGROUND_IMG = "img/long track.png";
extern int highScore;

enum GameState { MENU, PLAYING, GAME_OVER };

#endif
