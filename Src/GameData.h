#pragma once
/**
* @file GameData.h
*/
#include "Sprite.h"
#include "Font.h"

struct TitleScene;
struct GameOverScene;
struct MainScene;

extern const int windowWidth; // �E�B���h�E�̕�.
extern const int windowHeight; // �E�B���h�E�̍���.

extern SpriteRenderer renderer; // �X�v���C�g�`��p�ϐ�.
extern FontRenderer fontRenderer; // �t�H���g�`��p�ϐ�.

extern const int gamestateTitle = 0;
extern const int gamestateMain = 1;
extern const int gamestateGameover = 2;
extern int gamestate; // �Q�[���̏��.

extern TitleScene titleScene;
extern GameOverScene gameOverScene;
extern MainScene mainScene;
