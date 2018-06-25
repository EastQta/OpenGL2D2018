/**
* @file Main.cpp
*/
#include "GLFWEW.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include <random>

const char title[] = "OpenGL2D 2018"; // �E�B���h�E�^�C�g��.
const int windowWidth = 800; // �E�B���h�E�̕�.
const int windowHeight = 600; // �E�B���h�E�̍���.

/**
* �Q�[���L�����N�^�[�\����.
*/
struct Actor
{
  Sprite spr; // �X�v���C�g.
  Rect collisionShape; // �Փ˔���̈ʒu�Ƒ傫��.
  int health; // �ϋv��.
};

/*
* �Q�[���̕\���Ɋւ���ϐ�.
*/
SpriteRenderer renderer; // �X�v���C�g�`��p�ϐ�.
Sprite sprBackground; // �w�i�p�X�v���C�g.
Sprite sprPlayer;     // ���@�p�X�v���C�g.
glm::vec3 playerVelocity; // ���@�̈ړ����x.
FrameAnimation::TimelinePtr tlEnemy; // �G�̃A�j���[�V����.
FrameAnimation::TimelinePtr tlBlast; // �����̃A�j���[�V����.
Actor enemyList[128]; // �G�̃��X�g.
Actor playerBulletList[128]; // ���@�̒e�̃��X�g.
Actor blastList[128]; // �����̃��X�g.

Font::Renderer fontRenderer; // �t�H���g�`��p�ϐ�.

/*
* �Q�[���̃��[���Ɋւ���ϐ�.
*/
std::mt19937 random; // �����𔭐�������ϐ�(�����G���W��).
float enemyGenerationTimer; // ���̓G���o������܂ł̎���(�P��:�b).
int score; // �v���C���[�̃X�R�A.

/*
* �v���g�^�C�v�錾.
*/
void processInput(GLFWEW::WindowRef);
void update(GLFWEW::WindowRef);
void render(GLFWEW::WindowRef);
bool detectCollision(const Rect* lhs, const Rect* rhs);

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
  if (!Texture::Initialize()) {
    return 1;
  }
  if (!renderer.Initialize(1024)) {
    return 1;
  }

  random.seed(std::random_device()());
  fontRenderer.Init(1024, glm::vec2(windowWidth, windowHeight));
  fontRenderer.LoadFromFile("Res/Font/makinas_scrap.fnt");

  static const FrameAnimation::KeyFrame enemyAnime[] = {
    { 0.000f, glm::vec2(480, 0), glm::vec2(32, 32) },
    { 0.125f, glm::vec2(480, 96), glm::vec2(32, 32) },
    { 0.250f, glm::vec2(480, 64), glm::vec2(32, 32) },
    { 0.375f, glm::vec2(480, 32), glm::vec2(32, 32) },
    { 0.500f, glm::vec2(480, 0), glm::vec2(32, 32) },
  };
  tlEnemy = FrameAnimation::Timeline::Create(enemyAnime);

  static const FrameAnimation::KeyFrame blastAnime[] = {
    { 0.000f, glm::vec2(416,  0), glm::vec2(32, 32) },
    { 0.125f, glm::vec2(416, 32), glm::vec2(32, 32) },
    { 0.250f, glm::vec2(416, 64), glm::vec2(32, 32) },
    { 0.375f, glm::vec2(416, 96), glm::vec2(32, 32) },
    { 0.500f, glm::vec2(416, 96), glm::vec2(0, 0) },
  };
  tlBlast = FrameAnimation::Timeline::Create(blastAnime);

  // �g�p����摜��p��.
  sprBackground = Sprite("Res/UnknownPlanet.png");
  sprPlayer = Sprite("Res/Objects.png", glm::vec3(0, 0, 0), Rect(0, 0, 64, 32));

  for (Actor* i = std::begin(enemyList); i != std::end(enemyList); ++i) {
    i->health = 0;
  }
  for (Actor* i = std::begin(playerBulletList); i != std::end(playerBulletList); ++i) {
    i->health = 0;
  }
  for (Actor* i = std::begin(blastList); i != std::end(blastList); ++i) {
    i->health = 0;
  }

  // �Q�[�����[�v.
  while (!window.ShouldClose()) {
    processInput(window);
    update(window);
    render(window);
  }

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

  // ���@�̑��x��ݒ肷��.
  const GamePad gamepad = window.GetGamePad();
  if (gamepad.buttons & GamePad::DPAD_UP) {
    playerVelocity.y = 1;
  } else if (gamepad.buttons & GamePad::DPAD_DOWN) {
    playerVelocity.y = -1;
  } else {
    playerVelocity.y = 0;
  }
  if (gamepad.buttons & GamePad::DPAD_RIGHT) {
    playerVelocity.x = 1;
  } else if (gamepad.buttons & GamePad::DPAD_LEFT) {
    playerVelocity.x = -1;
  } else {
    playerVelocity.x = 0;
  }
  if (playerVelocity.x || playerVelocity.y) {
    playerVelocity = glm::normalize(playerVelocity) * 400.0f;
  }

  // �e�̔���.
  if (gamepad.buttonDown & GamePad::A) {
    Actor* bullet = nullptr;
    for (Actor* i = std::begin(playerBulletList); i != std::end(playerBulletList); ++i) {
      if (i->health <= 0) {
        bullet = i;
        break;
      }
    }
    bullet->spr = Sprite("Res/Objects.png", sprPlayer.Position(), Rect(64, 0, 32, 16));
    bullet->spr.Tweener(TweenAnimation::Animate::Create(TweenAnimation::MoveBy::Create(1, glm::vec3(1200, 0, 0))));
    bullet->collisionShape = Rect(-16, -8, 32, 16);
    bullet->health = 1;
  }
}

/**
* �Q�[���̏�Ԃ��X�V����.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
*/
void update(GLFWEW::WindowRef window)
{
  const float deltaTime = window.DeltaTime();

  // ���@�̈ړ�.
  if (playerVelocity.x || playerVelocity.y) {
    glm::vec3 newPos = sprPlayer.Position() + playerVelocity * deltaTime;
    const Rect playerRect = sprPlayer.Rectangle();
    if (newPos.x < -0.5f * (windowWidth - playerRect.size.x)) {
      newPos.x = -0.5f * (windowWidth - playerRect.size.x);
    } else if (newPos.x > 0.5f * (windowWidth - playerRect.size.x)) {
      newPos.x = 0.5f * (windowWidth - playerRect.size.x);
    }
    if (newPos.y < -0.5f * (windowHeight - playerRect.size.y)) {
      newPos.y = -0.5f * (windowHeight - playerRect.size.y);
    } else if (newPos.y > 0.5f * (windowHeight - playerRect.size.y)) {
      newPos.y = 0.5f * (windowHeight - playerRect.size.y);
    }
    sprPlayer.Position(newPos);
  }
  sprPlayer.Update(deltaTime);

  // �G�̏o��.
  enemyGenerationTimer -= deltaTime;
  if (enemyGenerationTimer < 0) {
    Actor* enemy = nullptr;
    for (Actor* i = std::begin(enemyList); i != std::end(enemyList); ++i) {
      if (i->health <= 0) {
        enemy = i;
        break;
      }
    }
    if (enemy) {
      const std::uniform_real_distribution<float> y_distribution(-0.5f * windowHeight, 0.5f * windowHeight);
      enemy->spr = Sprite("Res/Objects.png", glm::vec3(0.5f * windowWidth, y_distribution(random), 0), Rect(480, 0, 32, 32));
      enemy->spr.Animator(FrameAnimation::Animate::Create(tlEnemy));
      enemy->spr.Tweener(TweenAnimation::Animate::Create(TweenAnimation::MoveBy::Create(5, glm::vec3(-1000, 0, 0))));
      enemy->collisionShape = Rect(-16, -16, 32, 32);
      enemy->health = 1;
      enemyGenerationTimer = 2;
    }
  }

  // �G�̍X�V.
  for (Actor* enemy = std::begin(enemyList); enemy != std::end(enemyList); ++enemy) {
    if (enemy->health > 0) {
      enemy->spr.Update(deltaTime);
      if (enemy->spr.Tweener()->IsFinished()) {
        enemy->health = 0;
      }
    }
  }

  // ���@�̒e�̍X�V.
  for (auto bullet = std::begin(playerBulletList); bullet != std::end(playerBulletList); ++bullet) {
    // �����Ă������Ԃ��X�V.
    if (bullet->health > 0) {
      bullet->spr.Update(deltaTime);
      if (bullet->spr.Tweener()->IsFinished()) {
        bullet->health = 0;
      }
    }
  }

  // ���@�̒e�ƓG�̏Փ˔���.
  for (Actor* bullet = std::begin(playerBulletList); bullet != std::end(playerBulletList); ++bullet) {
    if (bullet->health <= 0) {
      continue;
    }
    Rect shotRect = bullet->collisionShape;
    shotRect.origin += glm::vec2(bullet->spr.Position());
    for (Actor* enemy = std::begin(enemyList); enemy != std::end(enemyList); ++enemy) {
      if (enemy->health <= 0) {
        continue;
      }
      Rect enemyRect = enemy->collisionShape;
      enemyRect.origin += glm::vec2(enemy->spr.Position());
      if (detectCollision(&shotRect, &enemyRect)) {
        bullet->health -= 1;
        enemy->health -= 1;
        score += 100;
        // �����G�t�F�N�g�𔭐�������.
        Actor* blast = nullptr;
        for (Actor* i = std::begin(blastList); i != std::end(blastList); ++i) {
          if (i->health <= 0) {
            blast = i;
            break;
          }
        }
        blast->spr = Sprite("Res/Objects.png", enemy->spr.Position());
        blast->spr.Animator(FrameAnimation::Animate::Create(tlBlast));
        blast->spr.Animator()->Loop(false);
        blast->health = 1;
        break;
      }
    }
  }

  // �����̍X�V.
  for (Actor* blast = std::begin(blastList); blast != std::end(blastList); ++blast) {
    if (blast->health > 0) {
      blast->spr.Update(deltaTime);
      if (blast->spr.Animator()->IsFinished()) {
        blast->health = 0;
      }
    }
  }
}

/**
* �Q�[���̏�Ԃ�`�悷��..
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
*/
void render(GLFWEW::WindowRef window)
{
  renderer.BeginUpdate();
  renderer.AddVertices(sprBackground);
  renderer.AddVertices(sprPlayer);
  for (const Actor* i = std::begin(enemyList); i != std::end(enemyList); ++i) {
    if (i->health > 0) {
      renderer.AddVertices(i->spr);
    }
  }
  for (const Actor* i = std::begin(playerBulletList); i != std::end(playerBulletList); ++i) {
    if (i->health > 0) {
      renderer.AddVertices(i->spr);
    }
  }
  for (const Actor* i = std::begin(blastList); i != std::end(blastList); ++i) {
    if (i->health > 0) {
      renderer.AddVertices(i->spr);
    }
  }
  renderer.EndUpdate();
  renderer.Draw({ windowWidth, windowHeight });

  fontRenderer.MapBuffer();
  std::wstring str;
  str.resize(8);
  int n = score;
  for (int i = 7; i >= 0; --i) {
    str[i] = L'0' + (n % 10);
    n /= 10;
  }
  fontRenderer.AddString(glm::vec2(-64 , 300), str.c_str());
  fontRenderer.UnmapBuffer();
  fontRenderer.Draw();

  window.SwapBuffers();
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
