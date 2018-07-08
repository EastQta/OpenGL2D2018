/**
* @file Main.cpp
*/
#include "GLFWEW.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TiledMap.h"
#include <glm/gtc/constants.hpp>
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
FontRenderer fontRenderer; // �t�H���g�`��p�ϐ�.
Sprite sprBackground; // �w�i�p�X�v���C�g.
Actor sprPlayer;     // ���@�p�X�v���C�g.
glm::vec3 playerVelocity; // ���@�̈ړ����x.
Actor enemyList[128]; // �G�̃��X�g.
Actor playerBulletList[128]; // ���@�̒e�̃��X�g.
Actor effectList[128]; // �����Ȃǂ̓�����ʗp�X�v���C�g�̃��X�g.

// �G�̃A�j���[�V����.
const FrameAnimation::KeyFrame enemyKeyFrames[] = {
  { 0.000f, glm::vec2(480, 0), glm::vec2(32, 32) },
  { 0.125f, glm::vec2(480, 96), glm::vec2(32, 32) },
  { 0.250f, glm::vec2(480, 64), glm::vec2(32, 32) },
  { 0.375f, glm::vec2(480, 32), glm::vec2(32, 32) },
  { 0.500f, glm::vec2(480, 0), glm::vec2(32, 32) },
};
FrameAnimation::TimelinePtr tlEnemy;

// �����A�j���[�V����.
const FrameAnimation::KeyFrame blastKeyFrames[] = {
  { 0 / 60.0f, glm::vec2(416, 0), glm::vec2(32, 32) },
  { 5 / 60.0f, glm::vec2(416, 32), glm::vec2(32, 32) },
  { 10 / 60.0f, glm::vec2(416, 64), glm::vec2(32, 32) },
  { 15 / 60.0f, glm::vec2(416, 96), glm::vec2(32, 32) },
  { 20 / 60.0f, glm::vec2(416, 96), glm::vec2(32, 32) },
};
FrameAnimation::TimelinePtr tlBlast;

// �G�̏o���𐧌䂷�邽�߂̃f�[�^.
TiledMap enemyMap;
float mapCurrentPosX;
float mapProcessedX;

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
void initializeActorList(Actor*, Actor*);
Actor* findAvailableActor(Actor*, Actor*);
void updateActorList(Actor*, Actor*, float deltaTime);
void renderActorList(const Actor* first, const Actor* last, SpriteRenderer* renderer);

using CollisionHandlerType = bool(*)(Actor*, Actor*);
void collisionDetection(Actor* first0, Actor* last0, Actor* first1, Actor* last1, CollisionHandlerType function);
bool playerBulletAndEnemyContactHandler(Actor * bullet, Actor * enemy);
bool playerAndEnemyContactHandler(Actor * player, Actor * enemy);

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
  if (!fontRenderer.Initialize(1024, glm::vec2(windowWidth, windowHeight))) {
    return 1;
  }
  if (!fontRenderer.LoadFromFile("Res/Font/makinas_scrap.fnt")) {
    return 1;
  }

  random.seed(std::random_device()()); // �����G���W���̏�����.

  tlEnemy = FrameAnimation::Timeline::Create(enemyKeyFrames);
  tlBlast = FrameAnimation::Timeline::Create(blastKeyFrames);

  // �g�p����摜��p��.
  sprBackground = Sprite("Res/UnknownPlanet.png");
  sprPlayer.spr = Sprite("Res/Objects.png", glm::vec3(0, 0, 0), Rect(0, 0, 64, 32));
  sprPlayer.collisionShape = Rect(-24, -8, 48, 16);
  sprPlayer.health = 1;

  initializeActorList(std::begin(enemyList), std::end(enemyList));
  initializeActorList(std::begin(playerBulletList), std::end(playerBulletList));
  initializeActorList(std::begin(effectList), std::end(effectList));
  enemyGenerationTimer = 2;

  score = 0;

  // �G�z�u�}�b�v��ǂݍ���.
  enemyMap.Load("Res/EnemyMap.json");
  mapCurrentPosX = mapProcessedX = windowWidth;

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

  if (sprPlayer.health > 0) {
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
      Actor* bullet = findAvailableActor(std::begin(playerBulletList), std::end(playerBulletList));
      if (bullet != nullptr) {
        bullet->spr = Sprite("Res/Objects.png", sprPlayer.spr.Position(), Rect(64, 0, 32, 16));
        bullet->spr.Tweener(TweenAnimation::Animate::Create(TweenAnimation::MoveBy::Create(1, glm::vec3(1200, 0, 0))));
        bullet->collisionShape = Rect(-16, -8, 32, 16);
        bullet->health = 1;
      }
    }
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
  if (sprPlayer.health > 0) {
    if (playerVelocity.x || playerVelocity.y) {
      glm::vec3 newPos = sprPlayer.spr.Position() + playerVelocity * deltaTime;
      const Rect playerRect = sprPlayer.spr.Rectangle();
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
      sprPlayer.spr.Position(newPos);
    }
    sprPlayer.spr.Update(deltaTime);
  }

  // �G�̏o��.
#if 1
  const TiledMap::Layer& tiledMapLayer = enemyMap.GetLayer(0);
  const glm::vec2 tileSize = enemyMap.GetTileSet(tiledMapLayer.tilesetNo).size;
  // �G�z�u�}�b�v�Q�ƈʒu�̍X�V.
  const float enemyMapScrollSpeed = 100; // �X�V���x.
  mapCurrentPosX += enemyMapScrollSpeed * deltaTime;
  if (mapCurrentPosX >= tiledMapLayer.size.x * tileSize.x) {
    // �I�[�𒴂�����擪�Ƀ��[�v.
    mapCurrentPosX = 0;
    mapProcessedX = 0;
  }
  // ���̗�ɓ��B������f�[�^��ǂ�.
  if (mapCurrentPosX - mapProcessedX >= tileSize.x) {
    mapProcessedX += tileSize.x;
    const int mapX = static_cast<int>(mapProcessedX / tileSize.x);
    for (int mapY = 0; mapY < tiledMapLayer.size.y; ++mapY) {
      const int enemyId = 256; // �G�Ƃ݂Ȃ��^�C��ID.
      if (tiledMapLayer.At(mapY, mapX) == enemyId) {
        Actor* enemy = findAvailableActor(std::begin(enemyList), std::end(enemyList));
        if (enemy != nullptr) {
          const float y = windowHeight * 0.5f - static_cast<float>(mapY * tileSize.x);
          enemy->spr = Sprite("Res/Objects.png", glm::vec3(0.5f * windowWidth, y, 0), Rect(480, 0, 32, 32));
          enemy->spr.Animator(FrameAnimation::Animate::Create(tlEnemy));
          namespace TA = TweenAnimation;
          TA::SequencePtr seq = TA::Sequence::Create(4);
          seq->Add(TA::MoveBy::Create(1, glm::vec3(0, 100, 0), TA::EasingType::EaseInOut, TA::Target::Y));
          seq->Add(TA::MoveBy::Create(1, glm::vec3(0, -100, 0), TA::EasingType::EaseInOut, TA::Target::Y));
          TA::ParallelizePtr par = TA::Parallelize::Create(1);
          par->Add(seq);
          par->Add(TA::MoveBy::Create(8, glm::vec3(-1000, 0, 0), TA::EasingType::Linear, TA::Target::X));
          enemy->spr.Tweener(TA::Animate::Create(par));
          enemy->collisionShape = Rect(-16, -16, 32, 32);
          enemy->health = 1;
        }
      }
    }
  }
#else
  enemyGenerationTimer -= deltaTime;
  if (enemyGenerationTimer < 0) {
    Actor* enemy = FindAvailableActor(std::begin(enemyList), std::end(enemyList));
    if (enemy != nullptr) {
      const std::uniform_real_distribution<float> y_distribution(-0.5f * windowHeight, 0.5f * windowHeight);
      enemy->spr = Sprite("Res/Objects.png", glm::vec3(0.5f * windowWidth, y_distribution(random), 0), Rect(480, 0, 32, 32));
      enemy->spr.Animator(FrameAnimation::Animate::Create(tlEnemy));
      namespace TA = TweenAnimation;
      TA::SequencePtr seq = TA::Sequence::Create(2);
      seq->Add(TA::MoveBy::Create(1, glm::vec3(-200, 100, 0), TA::EasingType::Linear));
      seq->Add(TA::MoveBy::Create(1, glm::vec3(-200, -100, 0), TA::EasingType::Linear));
      enemy->spr.Tweener(TweenAnimation::Animate::Create(seq));
      enemy->collisionShape = Rect(-16, -16, 32, 32);
      enemy->health = 1;
      enemyGenerationTimer = 2;
    }
  }
#endif

  // Actor�̍X�V.
  updateActorList(std::begin(enemyList), std::end(enemyList), deltaTime);
  updateActorList(std::begin(playerBulletList), std::end(playerBulletList), deltaTime);
  updateActorList(std::begin(effectList), std::end(effectList), deltaTime);

  // ���@�̒e�ƓG�̏Փ˔���.
  collisionDetection(
    std::begin(playerBulletList), std::end(playerBulletList),
    std::begin(enemyList), std::end(enemyList),
    playerBulletAndEnemyContactHandler);

  // ���@�ƓG�̏Փ˔���.
  collisionDetection(
    &sprPlayer, &sprPlayer + 1,
    std::begin(enemyList), std::end(enemyList),
    playerAndEnemyContactHandler
  );
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
  if (sprPlayer.health > 0) {
    renderer.AddVertices(sprPlayer.spr);
  }
  renderActorList(std::begin(enemyList), std::end(enemyList), &renderer);
  renderActorList(std::begin(playerBulletList), std::end(playerBulletList), &renderer);
  renderActorList(std::begin(effectList), std::end(effectList), &renderer);
  renderer.EndUpdate();
  renderer.Draw({ windowWidth, windowHeight });

  fontRenderer.BeginUpdate();
  char str[9];
  snprintf(str, 9, "%08d", score);
  fontRenderer.AddString(glm::vec2(-64 , 300), str);
  fontRenderer.EndUpdate();
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

/**
* Actor�̔z�������������.
*
* @param first �������Ώۂ̐擪�v�f�̃|�C���^.
* @param last  �������Ώۂ̏I�[�v�f�̃|�C���^.
*/
void initializeActorList(Actor* first, Actor* last)
{
  for (Actor* i = first; i != last; ++i) {
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
* ���@�̒e�ƓG�̏Փ˂���������.
*
* @param bullet ���@�̒e�̃|�C���^.
* @param enemy  �G�̃|�C���^.
*
* @retval true  �e�̏Փˏ������I������.
* @retval false �e�̏Փˏ������p������.
*/
bool playerBulletAndEnemyContactHandler(Actor * bullet, Actor * enemy)
{
  bullet->health -= 1;
  enemy->health -= 1;
  if (enemy->health <= 0) {
    score += 100;
    Actor* blast = findAvailableActor(std::begin(effectList), std::end(effectList));
    if (blast != nullptr) {
      blast->spr = Sprite("Res/Objects.png", enemy->spr.Position());
      blast->spr.Animator(FrameAnimation::Animate::Create(tlBlast));
      namespace TA = TweenAnimation;
      blast->spr.Tweener(TA::Animate::Create(TA::Rotation::Create(20 / 60.0f, glm::pi<float>() * 0.5f)));
      blast->health = 1;
    }
  }
  return bullet->health <= 0;
}

/**
* ���@�̒e�ƓG�̏Փ˂���������.
*
* @param bullet ���@�̃|�C���^.
* @param enemy  �G�̃|�C���^.
*
* @retval true  �e�̏Փˏ������I������.
* @retval false �e�̏Փˏ������p������.
*/
bool playerAndEnemyContactHandler(Actor * player, Actor * enemy)
{
  if (player->health > enemy->health) {
    player->health -= enemy->health;
    enemy->health = 0;;
  } else {
    enemy->health -= player->health;
    player->health = 0;
  }
  if (enemy->health <= 0) {
    score += 100;
    Actor* blast = findAvailableActor(std::begin(effectList), std::end(effectList));
    if (blast != nullptr) {
      blast->spr = Sprite("Res/Objects.png", enemy->spr.Position());
      blast->spr.Animator(FrameAnimation::Animate::Create(tlBlast));
      namespace TA = TweenAnimation;
      blast->spr.Tweener(TA::Animate::Create(TA::Rotation::Create(20 / 60.0f, glm::pi<float>() * 0.5f)));
      blast->health = 1;
    }
  }
  if (player->health <= 0) {
    Actor* blast = findAvailableActor(std::begin(effectList), std::end(effectList));
    if (blast != nullptr) {
      blast->spr = Sprite("Res/Objects.png", enemy->spr.Position());
      blast->spr.Animator(FrameAnimation::Animate::Create(tlBlast));
      namespace TA = TweenAnimation;
      blast->spr.Tweener(TA::Animate::Create(TA::Rotation::Create(20 / 60.0f, glm::pi<float>() * 0.5f)));
      blast->spr.Scale(glm::vec2(2, 2));
      blast->health = 1;
    }
  }
  return player->health <= 0;
}

/**
* �Փ˔���.
*/
void collisionDetection(Actor* first0, Actor* last0, Actor* first1, Actor* last1, CollisionHandlerType function)
{
  for (Actor* bullet = first0; bullet != last0; ++bullet) {
    if (bullet->health <= 0) {
      continue;
    }
    Rect shotRect = bullet->collisionShape;
    shotRect.origin += glm::vec2(bullet->spr.Position());
    for (Actor* enemy = first1; enemy != last1; ++enemy) {
      if (enemy->health <= 0) {
        continue;
      }
      Rect enemyRect = enemy->collisionShape;
      enemyRect.origin += glm::vec2(enemy->spr.Position());
      if (detectCollision(&shotRect, &enemyRect)) {
        if (function(bullet, enemy)) {
          break;
        }
      }
    }
  }
}