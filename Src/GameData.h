#pragma once
/**
* @file GameData.h
*/
#include "Sprite.h"
#include "Font.h"

struct TitleScene;
struct GameOverScene;
struct MainScene;

extern SpriteRenderer renderer; // �X�v���C�g�`��p�ϐ�.
extern FontRenderer fontRenderer; // �t�H���g�`��p�ϐ�.

const int gamestateTitle = 0;
const int gamestateMain = 1;
const int gamestateGameover = 2;
extern int gamestate; // �Q�[���̏��.

extern TitleScene titleScene;
extern GameOverScene gameOverScene;
extern MainScene mainScene;
