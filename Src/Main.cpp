/**
* @file Main.cpp
*/
#include "TitleSceneh.h"
#include "GameOverScene.h"
#include "MainScene.h"
#include "GameData.h"
#include "GLFWEW.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TiledMap.h"
#include "Audio.h"
#include <glm/gtc/constants.hpp>
#include <random>

const char title[] = "OpenGL2D 2018"; // �E�B���h�E�^�C�g��.
const int windowWidth = 800; // �E�B���h�E�̕�.
const int windowHeight = 600; // �E�B���h�E�̍���.

/*
* �Q�[���̕\���Ɋւ���ϐ�.
*/
SpriteRenderer renderer; // �X�v���C�g�`��p�ϐ�.
FontRenderer fontRenderer; // �t�H���g�`��p�ϐ�.

/*
* �Q�[���̃��[���Ɋւ���ϐ�.
*/
std::mt19937 random; // �����𔭐�������ϐ�(�����G���W��).

/*
* �v���g�^�C�v�錾.
*/
void processInput(GLFWEW::WindowRef);
void update(GLFWEW::WindowRef);
void render(GLFWEW::WindowRef);

int gamestate; // �Q�[���̏��.

TitleScene titleScene;
GameOverScene gameOverScene;
MainScene mainScene;

/**
* �v���O�����̃G���g���[�|�C���g.
*/
int main()
{
  // �A�v���P�[�V�����̏�����.
  GLFWEW::WindowRef window = GLFWEW::Window::Instance();
  if (!window.Initialize(windowWidth, windowHeight, title)) {
    return 1;
  }
  Audio::EngineRef audio = Audio::Engine::Instance();
  if (!audio.Initialize()) {
    return 1;
  }
  if (!Texture::Initialize()) {
    return 1;
  }
  if (!renderer.Initialize(1024)) {
    return 1;
  }
  if (!fontRenderer.Initialize(1024, glm::vec2(windowWidth, windowHeight))) {
    return 1;
  }
  if (!fontRenderer.LoadFromFile("Res/Font/makinas_scrap.fnt")) {
    return 1;
  }

  random.seed(std::random_device()()); // �����G���W���̏�����.

  initialize(&titleScene);

  // �Q�[�����[�v.
  while (!window.ShouldClose()) {
    processInput(window);
    update(window);
    render(window);
    audio.Update();
  }

  audio.Destroy();
  Texture::Finalize();
  return 0;
}

/**
* �v���C���[�̓��͂���������.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
*/
void processInput(GLFWEW::WindowRef window)
{
  window.Update();

  if (gamestate == gamestateTitle) {
    processInput(window, &titleScene);
    return;
  } else if (gamestate == gamestateGameover) {
    processInput(window, &gameOverScene);
    return;
  } else if (gamestate == gamestateMain) {
    processInput(window, &mainScene);
  }
}

/**
* �Q�[���̏�Ԃ��X�V����.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
*/
void update(GLFWEW::WindowRef window)
{
  if (gamestate == gamestateTitle) {
    update(window, &titleScene);
    return;
  } else if (gamestate == gamestateGameover) {
    update(window, &gameOverScene);
    return;
  } else if (gamestate == gamestateMain) {
    update(window, &mainScene);
    if (mainScene.sprPlayer.health <= 0) {
      finalize(&mainScene);
      gamestate = gamestateGameover;
      initialize(&gameOverScene);
      return;
    }
  }
}

/**
* �Q�[���̏�Ԃ�`�悷��.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
*/
void render(GLFWEW::WindowRef window)
{
  if (gamestate == gamestateTitle) {
    render(window, &titleScene);
    return;
  } else if (gamestate == gamestateGameover) {
    render(window, &gameOverScene);
    return;
  } else if (gamestate == gamestateMain) {
    render(window, &mainScene);
    return;
  }
}
