#pragma once
/**
* @file TitleScene.h
*/
#include "GLFWEW.h"
#include "Sprite.h"

/**
* �^�C�g����ʂŎg�p����\����.
*/
struct TitleScene
{
  Sprite bg;
  Sprite logo;
  const int modeStart = 0;
  const int modeTitle = 1;
  const int modeNextState = 2;
  int mode;
  float timer;
};
bool initialize(TitleScene*);
void finalize(TitleScene*);
void processInput(GLFWEW::WindowRef, TitleScene*);
void update(GLFWEW::WindowRef, TitleScene*);
void render(GLFWEW::WindowRef, TitleScene*);
