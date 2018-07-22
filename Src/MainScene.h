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

  float enemyGenerationTimer; // ���̓G���o������܂ł̎���(�P��:�b).
  int score; // �v���C���[�̃X�R�A.
  Audio::SoundPtr bgm; // BGM����p�ϐ�.
  Audio::SoundPtr seBlast;
  Audio::SoundPtr sePlayerShot;

  // �G�̏o���𐧌䂷�邽�߂̃f�[�^.
  TiledMap enemyMap;
  float mapCurrentPosX;
  float mapProcessedX;
};
bool initialize(MainScene*);
void finalize(MainScene*);
void processInput(GLFWEW::WindowRef, MainScene*);
void update(GLFWEW::WindowRef, MainScene*);
void render(GLFWEW::WindowRef, MainScene*);
