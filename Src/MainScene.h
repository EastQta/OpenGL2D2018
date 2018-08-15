#pragma once
/**
* @file MainScene.h
*/
#include "GLFWEW.h"
#include "Sprite.h"
#include "Audio.h"
#include "TiledMap.h"
#include "Actor.h"

/**
* ���C����ʂŎg�p����\����.
*/
struct MainScene
{
  FrameAnimation::TimelinePtr tlEnemy;
  FrameAnimation::TimelinePtr tlBlast;

  Sprite sprBackground; // �w�i�p�X�v���C�g.
  Actor sprPlayer;     // ���@�p�X�v���C�g.
  glm::vec3 playerVelocity; // ���@�̈ړ����x.
  Actor enemyList[128]; // �G�̃��X�g.
  Actor playerBulletList[128]; // ���@�̒e�̃��X�g.
  Actor effectList[128]; // �����Ȃǂ̓�����ʗp�X�v���C�g�̃��X�g.
  Actor itemList[32]; // �p���[�A�b�v�⓾�_�A�C�e���̃��X�g.

  float enemyGenerationTimer; // ���̓G���o������܂ł̎���(�P��:�b).
  int score; // �v���C���[�̃X�R�A.

  // ����̎��.
  const int weaponNormalShot = 0;
  const int weaponLaser = 1;

  int weapon; // ���@�̕���.

  const int weaponLevelMin = 0;
  const int weaponLevelMax = 5;
  int weaponLevel; // ����̋����x.

  float shotTimer; // �e�̔��ˊԊu.

  const int laserLength = 16;
  int laserCount; // ���˒��̃��[�U�[�̒���.
  Actor* laserBack; // ���[�U�[�̖���.
  float laserPosX; // ���[�U�[���˒n�_��X���W(Y���W�͎��@�ɒǐ�����̂ŕs�v).

  Audio::SoundPtr bgm; // BGM����p�ϐ�.
  Audio::SoundPtr seBlast;
  Audio::SoundPtr sePlayerShot;
  Audio::SoundPtr sePlayerLaser;
  Audio::SoundPtr seItem;

  // �G�̏o���𐧌䂷�邽�߂̃f�[�^.
  TiledMap enemyMap;
  float mapCurrentPosX;
  float mapProcessedX;

  float timer;
};
bool initialize(MainScene*);
void finalize(MainScene*);
void processInput(GLFWEW::WindowRef, MainScene*);
void update(GLFWEW::WindowRef, MainScene*);
void render(GLFWEW::WindowRef, MainScene*);
