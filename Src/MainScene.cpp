/**
* @file MainScene.cpp
*/
#include "MainScene.h"
#include "GameOverScene.h"
#include "GameData.h"
#include <glm/gtc/constants.hpp>

// �G�̃A�j���[�V����.
const FrameAnimation::KeyFrame enemyKeyFrames[] = {
  { 0.000f, glm::vec2(480, 0), glm::vec2(32, 32) },
  { 0.125f, glm::vec2(480, 96), glm::vec2(32, 32) },
  { 0.250f, glm::vec2(480, 64), glm::vec2(32, 32) },
  { 0.375f, glm::vec2(480, 32), glm::vec2(32, 32) },
  { 0.500f, glm::vec2(480, 0), glm::vec2(32, 32) },
};

// �����A�j���[�V����.
const FrameAnimation::KeyFrame blastKeyFrames[] = {
  { 0 / 60.0f, glm::vec2(416, 0), glm::vec2(32, 32) },
  { 5 / 60.0f, glm::vec2(416, 32), glm::vec2(32, 32) },
  { 10 / 60.0f, glm::vec2(416, 64), glm::vec2(32, 32) },
  { 15 / 60.0f, glm::vec2(416, 96), glm::vec2(32, 32) },
  { 20 / 60.0f, glm::vec2(416, 96), glm::vec2(32, 32) },
};

void playerBulletAndEnemyContactHandler(Actor * bullet, Actor * enemy);
void playerAndEnemyContactHandler(Actor * player, Actor * enemy);

/**
* ���C����ʗp�̍\���̂̏����ݒ���s��.
*
* @param scene     ���C����ʗp�\���̂̃|�C���^.
* @param gamestate �Q�[����Ԃ�\���ϐ��̃|�C���^.
*
* @retval true  ����������.
* @retval false ���������s.
*/
bool initialize(MainScene* scene)
{
  scene->tlEnemy = FrameAnimation::Timeline::Create(enemyKeyFrames);
  scene->tlBlast = FrameAnimation::Timeline::Create(blastKeyFrames);

  scene->sprBackground = Sprite("Res/UnknownPlanet.png");
  scene->sprPlayer.spr = Sprite("Res/Objects.png", glm::vec3(0, 0, 0), Rect(0, 0, 64, 32));
  scene->sprPlayer.collisionShape = Rect(-24, -8, 48, 16);
  scene->sprPlayer.health = 1;

  initializeActorList(std::begin(scene->enemyList), std::end(scene->enemyList));
  initializeActorList(std::begin(scene->playerBulletList), std::end(scene->playerBulletList));
  initializeActorList(std::begin(scene->effectList), std::end(scene->effectList));

  scene->score = 0;

  scene->enemyMap.Load("Res/EnemyMap.json");
  scene->mapCurrentPosX = scene->mapProcessedX = (float)GLFWEW::Window::Instance().Width();

  Audio::EngineRef audio = Audio::Engine::Instance();
  scene->seBlast = audio.Prepare("Res/Audio/Blast.xwm");
  scene->sePlayerShot = audio.Prepare("Res/Audio/PlayerShot.xwm");
  scene->bgm = audio.Prepare(L"Res/Audio/Neolith.xwm");
  scene->bgm->Play(Audio::Flag_Loop);

  return true;
}

/**
* ���C����ʂ̏I���������s��.
*
* @param scene  ���C����ʗp�\���̂̃|�C���^.
*/
void finalize(MainScene* scene)
{
  scene->bgm->Stop();
  scene->seBlast.reset();
  scene->sePlayerShot.reset();
  scene->bgm.reset();

  scene->enemyMap.Unload();

  initializeActorList(std::begin(scene->enemyList), std::end(scene->enemyList));
  initializeActorList(std::begin(scene->playerBulletList), std::end(scene->playerBulletList));
  initializeActorList(std::begin(scene->effectList), std::end(scene->effectList));

  scene->sprBackground = Sprite();
  scene->sprPlayer.spr = Sprite();

  scene->tlEnemy.reset();
  scene->tlBlast.reset();
}

/**
* �v���C���[�̓��͂���������.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
* @param scene  ���C����ʗp�\���̂̃|�C���^.
*/
void processInput(GLFWEW::WindowRef window, MainScene* scene)
{
  if (scene->sprPlayer.health <= 0) {
    scene->playerVelocity = glm::vec3(0, 0, 0);
  } else {
    // ���@�̑��x��ݒ肷��.
    const GamePad gamepad = window.GetGamePad();
    if (gamepad.buttons & GamePad::DPAD_UP) {
      scene->playerVelocity.y = 1;
    } else if (gamepad.buttons & GamePad::DPAD_DOWN) {
      scene->playerVelocity.y = -1;
    } else {
      scene->playerVelocity.y = 0;
    }
    if (gamepad.buttons & GamePad::DPAD_RIGHT) {
      scene->playerVelocity.x = 1;
    } else if (gamepad.buttons & GamePad::DPAD_LEFT) {
      scene->playerVelocity.x = -1;
    } else {
      scene->playerVelocity.x = 0;
    }
    if (scene->playerVelocity.x || scene->playerVelocity.y) {
      scene->playerVelocity = glm::normalize(scene->playerVelocity) * 400.0f;
    }

    // �e�̔���.
    if (gamepad.buttonDown & GamePad::A) {
      scene->sePlayerShot->Play();
      Actor* bullet = findAvailableActor(std::begin(scene->playerBulletList), std::end(scene->playerBulletList));
      if (bullet != nullptr) {
        bullet->spr = Sprite("Res/Objects.png", scene->sprPlayer.spr.Position(), Rect(64, 0, 32, 16));
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
* @param scene  ���C����ʗp�\���̂̃|�C���^.
*/
void update(GLFWEW::WindowRef window, MainScene* scene)
{
  const float deltaTime = window.DeltaTime();

  // ���@�̈ړ�.
  if (scene->sprPlayer.health > 0) {
    if (scene->playerVelocity.x || scene->playerVelocity.y) {
      const GLFWEW::WindowRef window = GLFWEW::Window::Instance();
      const int windowWidth = window.Width();
      const int windowHeight = window.Height();
      glm::vec3 newPos = scene->sprPlayer.spr.Position() + scene->playerVelocity * deltaTime;
      const Rect playerRect = scene->sprPlayer.spr.Rectangle();
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
      scene->sprPlayer.spr.Position(newPos);
    }
    scene->sprPlayer.spr.Update(deltaTime);
  }

  // �G�̏o��.
#if 1
  const TiledMap::Layer& tiledMapLayer = scene->enemyMap.GetLayer(0);
  const glm::vec2 tileSize = scene->enemyMap.GetTileSet(tiledMapLayer.tilesetNo).size;
  // �G�z�u�}�b�v�Q�ƈʒu�̍X�V.
  const float enemyMapScrollSpeed = 100; // �X�V���x.
  scene->mapCurrentPosX += enemyMapScrollSpeed * deltaTime;
  if (scene->mapCurrentPosX >= tiledMapLayer.size.x * tileSize.x) {
    // �I�[�𒴂�����擪�Ƀ��[�v.
    scene->mapCurrentPosX = 0;
    scene->mapProcessedX = 0;
  }
  // ���̗�ɓ��B������f�[�^��ǂ�.
  if (scene->mapCurrentPosX - scene->mapProcessedX >= tileSize.x) {
    scene->mapProcessedX += tileSize.x;
    const int mapX = static_cast<int>(scene->mapProcessedX / tileSize.x);
    for (int mapY = 0; mapY < tiledMapLayer.size.y; ++mapY) {
      const int enemyId = 256; // �G�Ƃ݂Ȃ��^�C��ID.
      if (tiledMapLayer.At(mapY, mapX) == enemyId) {
        Actor* enemy = findAvailableActor(std::begin(scene->enemyList), std::end(scene->enemyList));
        if (enemy != nullptr) {
          const float y = window.Height() * 0.5f - static_cast<float>(mapY * tileSize.x);
          enemy->spr = Sprite("Res/Objects.png", glm::vec3(0.5f * window.Width(), y, 0), Rect(480, 0, 32, 32));
          enemy->spr.Animator(FrameAnimation::Animate::Create(scene->tlEnemy));
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
  updateActorList(std::begin(scene->enemyList), std::end(scene->enemyList), deltaTime);
  updateActorList(std::begin(scene->playerBulletList), std::end(scene->playerBulletList), deltaTime);
  updateActorList(std::begin(scene->effectList), std::end(scene->effectList), deltaTime);

  // ���@�̒e�ƓG�̏Փ˔���.
  detectCollision(
    std::begin(scene->playerBulletList), std::end(scene->playerBulletList),
    std::begin(scene->enemyList), std::end(scene->enemyList),
    playerBulletAndEnemyContactHandler);

  // ���@�ƓG�̏Փ˔���.
  detectCollision(
    &scene->sprPlayer, &scene->sprPlayer + 1,
    std::begin(scene->enemyList), std::end(scene->enemyList),
    playerAndEnemyContactHandler
  );
}

/**
* �Q�[���̏�Ԃ�`�悷��.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
* @param scene  ���C����ʗp�\���̂̃|�C���^.
*/
void render(GLFWEW::WindowRef window, MainScene* scene)
{
  renderer.BeginUpdate();
  renderer.AddVertices(scene->sprBackground);
  if (scene->sprPlayer.health > 0) {
    renderer.AddVertices(scene->sprPlayer.spr);
  }
  renderActorList(std::begin(scene->enemyList), std::end(scene->enemyList), &renderer);
  renderActorList(std::begin(scene->playerBulletList), std::end(scene->playerBulletList), &renderer);
  renderActorList(std::begin(scene->effectList), std::end(scene->effectList), &renderer);
  renderer.EndUpdate();
  renderer.Draw({ window.Width(), window.Height() });

  fontRenderer.BeginUpdate();
  char str[9];
  snprintf(str, 9, "%08d", scene->score);
  fontRenderer.AddString(glm::vec2(-64, 300), str);
  fontRenderer.EndUpdate();
  fontRenderer.Draw();

  window.SwapBuffers();
}

/**
* ���@�̒e�ƓG�̏Փ˂���������.
*
* @param bullet ���@�̒e�̃|�C���^.
* @param enemy  �G�̃|�C���^.
*/
void playerBulletAndEnemyContactHandler(Actor * bullet, Actor * enemy)
{
  bullet->health -= 1;
  enemy->health -= 1;
  if (enemy->health <= 0) {
    mainScene.score += 100;
    mainScene.seBlast->Play();
    Actor* blast = findAvailableActor(std::begin(mainScene.effectList), std::end(mainScene.effectList));
    if (blast != nullptr) {
      blast->spr = Sprite("Res/Objects.png", enemy->spr.Position());
      blast->spr.Animator(FrameAnimation::Animate::Create(mainScene.tlBlast));
      namespace TA = TweenAnimation;
      blast->spr.Tweener(TA::Animate::Create(TA::Rotation::Create(20 / 60.0f, glm::pi<float>() * 0.5f)));
      blast->health = 1;
    }
  }
}

/**
* ���@�̒e�ƓG�̏Փ˂���������.
*
* @param bullet ���@�̃|�C���^.
* @param enemy  �G�̃|�C���^.
*/
void playerAndEnemyContactHandler(Actor * player, Actor * enemy)
{
  if (player->health > enemy->health) {
    player->health -= enemy->health;
    enemy->health = 0;;
  } else {
    enemy->health -= player->health;
    player->health = 0;
  }
  if (enemy->health <= 0) {
    mainScene.score += 100;
    Actor* blast = findAvailableActor(std::begin(mainScene.effectList), std::end(mainScene.effectList));
    if (blast != nullptr) {
      blast->spr = Sprite("Res/Objects.png", enemy->spr.Position());
      blast->spr.Animator(FrameAnimation::Animate::Create(mainScene.tlBlast));
      namespace TA = TweenAnimation;
      blast->spr.Tweener(TA::Animate::Create(TA::Rotation::Create(20 / 60.0f, glm::pi<float>() * 0.5f)));
      blast->health = 1;
    }
  }
  if (player->health <= 0) {
    Actor* blast = findAvailableActor(std::begin(mainScene.effectList), std::end(mainScene.effectList));
    if (blast != nullptr) {
      blast->spr = Sprite("Res/Objects.png", enemy->spr.Position());
      blast->spr.Animator(FrameAnimation::Animate::Create(mainScene.tlBlast));
      namespace TA = TweenAnimation;
      blast->spr.Tweener(TA::Animate::Create(TA::Rotation::Create(20 / 60.0f, glm::pi<float>() * 0.5f)));
      blast->spr.Scale(glm::vec2(2, 2));
      blast->health = 1;
    }
  }
}
