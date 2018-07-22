#pragma once
/**
* @file Actor.h
*/
#include "Sprite.h"

/**
* �Q�[���L�����N�^�[�\����.
*/
struct Actor
{
  Sprite spr; // �X�v���C�g.
  Rect collisionShape; // �Փ˔���̈ʒu�Ƒ傫��.
  int health; // �ϋv��.
  int type; // ���.
};

bool detectCollision(const Rect* lhs, const Rect* rhs);
void initializeActorList(Actor*, Actor*);
Actor* findAvailableActor(Actor*, Actor*);
void updateActorList(Actor*, Actor*, float deltaTime);
void renderActorList(const Actor* first, const Actor* last, SpriteRenderer* renderer);

using CollisionHandlerType = void(*)(Actor*, Actor*);
void detectCollision(Actor* first0, Actor* last0, Actor* first1, Actor* last1, CollisionHandlerType function);
