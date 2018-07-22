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

/**
* 2�̒����`�̏Փˏ�Ԃ𒲂ׂ�.
*
* @param lhs �����`����1.
* @param rhs �����`����2.
*
* @retval true  �Փ˂��Ă���.
* @retval false �Փ˂��Ă��Ȃ�.
*/
bool detectCollision(const Rect* lhs, const Rect* rhs)
{
  return
    lhs->origin.x < rhs->origin.x + rhs->size.x &&
    lhs->origin.x + lhs->size.x > rhs->origin.x &&
    lhs->origin.y < rhs->origin.y + rhs->size.y &&
    lhs->origin.y + lhs->size.y > rhs->origin.y;
}

/**
* Actor�̔z�������������.
*
* @param first �������Ώۂ̐擪�v�f�̃|�C���^.
* @param last  �������Ώۂ̏I�[�v�f�̃|�C���^.
*/
void initializeActorList(Actor* first, Actor* last)
{
  for (Actor* i = first; i != last; ++i) {
    i->spr = Sprite();
    i->health = 0;
  }
}

/**
* ���p�\�Ȃ�Actor���擾����.
*
* @param first �����Ώۂ̐擪�v�f�̃|�C���^.
* @param last  �����Ώۂ̏I�[�v�f�̃|�C���^.
*
* @return ���p�\��Actor�̃|�C���^.
*         ���p�\��Actor��������Ȃ����nullptr.
*
* [first, last)�͈̔͂���A���p�\��(health��0�ȉ���)Actor����������.
*/
Actor* findAvailableActor(Actor* first, Actor* last)
{
  for (Actor* i = first; i != last; ++i) {
    if (i->health <= 0) {
      return i;
    }
  }
  return nullptr;
}

/**
* Actor�̔z����X�V����.
*
* @param first     �X�V�Ώۂ̐擪�v�f�̃|�C���^.
* @param last      �X�V�Ώۂ̏I�[�v�f�̃|�C���^.
* @param deltaTime �O��̍X�V����̌o�ߎ���.
*/
void updateActorList(Actor* first, Actor* last, float deltaTime)
{
  for (Actor* i = first; i != last; ++i) {
    if (i->health > 0) {
      i->spr.Update(deltaTime);
      if (i->spr.Tweener()->IsFinished()) {
        i->health = 0;
      }
    }
  }
}

/**
* Actor�̔z���`�悷��.
*
* @param first    �`��Ώۂ̐擪�v�f�̃|�C���^.
* @param last     �`��Ώۂ̏I�[�v�f�̃|�C���^.
" @param renderer �X�v���C�g�`��p�̕ϐ�.
*/
void renderActorList(const Actor* first, const Actor* last, SpriteRenderer* renderer)
{
  for (const Actor* i = first; i != last; ++i) {
    if (i->health > 0) {
      renderer->AddVertices(i->spr);
    }
  }
}

/**
* �Փ˂����o����.
*
* @param firstA    �Փ˂�����z��A�̐擪�|�C���^.
* @param lastA     �Փ˂�����z��A�̏I�[�|�C���^.
* @param firstB    �Փ˂�����z��B�̐擪�|�C���^.
* @param lastB     �Փ˂�����z��B�̏I�[�|�C���^.
* @param function  A-B�ԂŏՓ˂����o���ꂽ�Ƃ��Ɏ��s����֐�.
*/
void detectCollision(Actor* firstA, Actor* lastA, Actor* firstB, Actor* lastB, CollisionHandlerType function)
{
  for (Actor* a = firstA; a != lastA; ++a) {
    if (a->health <= 0) {
      continue;
    }
    Rect rectA = a->collisionShape;
    rectA.origin += glm::vec2(a->spr.Position());
    for (Actor* b = firstB; b != lastB; ++b) {
      if (b->health <= 0) {
        continue;
      }
      Rect rectB = b->collisionShape;
      rectB.origin += glm::vec2(b->spr.Position());
      if (detectCollision(&rectA, &rectB)) {
        function(a, b);
        if (a->health <= 0) {
          break;
        }
      }
    }
  }
}
