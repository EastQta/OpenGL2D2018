/**
* @file Actor.cpp
*/
#include "Actor.h"

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
