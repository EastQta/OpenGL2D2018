/**
* @file GameOverScene.cpp
*/
#include "GameOverScene.h"
#include "TitleSceneh.h"
#include "GameData.h"

/**
* �Q�[���I�[�o�[��ʂ̏����ݒ���s��.
*
* @param scene     �Q�[���I�[�o�[��ʗp�\���̂̃|�C���^.
*
* @retval true  ����������.
* @retval false ���������s.
*/
bool initialize(GameOverScene* scene)
{
  scene->bg = Sprite("Res/UnknownPlanet.png");
  scene->timer = 0.5f; // ���͂��󂯕t���Ȃ�����(�b).
  return true;
}

/**
* �Q�[���I�[�o�[��ʂ̏I���������s��.
*
* @param scene  �Q�[���I�[�o�[��ʗp�\���̂̃|�C���^.
*/
void finalize(GameOverScene* scene)
{
  scene->bg = Sprite();
}

/**
* �Q�[���I�[�o�[��ʂ̃v���C���[���͂���������.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
* @param scene  �Q�[���I�[�o�[��ʗp�\���̂̃|�C���^.
*/
void processInput(GLFWEW::WindowRef window, GameOverScene* scene)
{
  if (scene->timer <= 0) {
    const GamePad gamepad = window.GetGamePad();
    if (gamepad.buttonDown & GamePad::A) {
      gamestate = gamestateTitle;
      initialize(&titleScene);
      finalize(scene);
    }
  }
}

/**
* �Q�[���I�[�o�[��ʂ��X�V����.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
* @param scene  �Q�[���I�[�o�[��ʗp�\���̂̃|�C���^.
*/
void update(GLFWEW::WindowRef window, GameOverScene* scene)
{
  const float deltaTime = window.DeltaTime();
  if (scene->timer > 0) {
    scene->timer -= deltaTime;
  }
  scene->bg.Update(deltaTime);
}

/**
* �Q�[���I�[�o�[��ʂ��X�V����.
*
* @param window �Q�[�����Ǘ�����E�B���h�E.
* @param scene  �Q�[���I�[�o�[��ʗp�\���̂̃|�C���^.
*/
void render(GLFWEW::WindowRef window, GameOverScene* scene)
{
  renderer.BeginUpdate();
  renderer.AddVertices(scene->bg);
  renderer.EndUpdate();
  renderer.Draw(glm::vec2(window.Width(), window.Height()));

  fontRenderer.BeginUpdate();
  fontRenderer.AddString(glm::vec2(-144, 16), "GAME OVER");
  fontRenderer.EndUpdate();
  fontRenderer.Draw();

  window.SwapBuffers();
}