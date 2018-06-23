/**
* @file Main.cpp
*/
#include "GLFWEW.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include <list>
#include <random>

const char title[] = "OpenGL2D 2018"; // �E�B���h�E�^�C�g��.
const int windowWidth = 800; // �E�B���h�E�̕�.
const int windowHeight = 600; // �E�B���h�E�̍���.

Font::Renderer fontRenderer;
int score;

SpriteRenderer renderer;
Sprite sprBackground; // �w�i�p�X�v���C�g.
Sprite sprPlayer;     // ���@�p�X�v���C�g.
glm::vec3 playerVelocity; // ���@�̈ړ����x.

/**
* �Q�[���L�����N�^�[.
*/
struct Actor
{
  Actor(const char* texname, const glm::vec3& pos, const Rect& r, const glm::vec3& vel, const Rect& col)
  {
    spr = Sprite(texname, pos, r);
    velocity = vel;
    collision = col;
  }

  Sprite spr;
  glm::vec3 velocity;
  Rect collision;
  int health;
};
std::list<Actor> playerBulletList;
std::list<Actor> enemyList;

std::mt19937 random;
float enemyGenerationTimer;

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

  // �g�p����摜��p��.
  sprBackground = Sprite("Res/UnknownPlanet.png");
  sprPlayer = Sprite("Res/Objects.png", glm::vec3(0, 0, 0), Rect(0, 0, 64, 32));

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
    Actor shot = Actor("Res/Objects.png", sprPlayer.Position(), Rect(64, 0, 32, 16), glm::vec3(1200, 0, 0), Rect(-16, -8, 32, 16));
    shot.health = 1;
    playerBulletList.push_back(shot);
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

  // ���@�̒e�̈ړ�.
  for (auto i = playerBulletList.begin(); i != playerBulletList.end(); ++i) {
    i->spr.Position(i->spr.Position() + i->velocity * deltaTime);
    i->spr.Update(deltaTime);
  }
  playerBulletList.remove_if([](const Actor& e) { return e.spr.Position().x > (0.5f * (windowWidth + e.spr.Rectangle().size.x)); });

  // �G�̏o��.
  enemyGenerationTimer -= deltaTime;
  if (enemyGenerationTimer < 0) {
    const float y = std::uniform_real_distribution<float>(-0.5f * windowHeight, 0.5f * windowHeight)(random);
    Actor enemy = Actor("Res/Objects.png", glm::vec3(432, y, 0), Rect(480, 0, 32, 32), glm::vec3(-200, 0, 0), Rect(-16, -16, 32, 32));
    enemy.health = 1;
    enemyList.push_back(enemy);
    enemyGenerationTimer = 2;
  }

  // �G�̈ړ�.
  for (auto i = enemyList.begin(); i != enemyList.end(); ++i) {
    i->spr.Position(i->spr.Position() + i->velocity * deltaTime);
    i->spr.Update(deltaTime);
  }
  enemyList.remove_if([](const Actor& e) { return e.spr.Position().x < (-0.5f * (windowWidth + e.spr.Rectangle().size.x)); });

  // ���@�̒e�ƓG�̏Փ˔���.
  for (auto shot = playerBulletList.begin(); shot != playerBulletList.end(); ++shot) {
    if (shot->health <= 0) {
      continue;
    }
    Rect shotRect = shot->collision;
    shotRect.origin += glm::vec2(shot->spr.Position());
    for (auto enemy = enemyList.begin(); enemy != enemyList.end(); ++enemy) {
      if (enemy->health <= 0) {
        continue;
      }
      Rect enemyRect = enemy->collision;
      enemyRect.origin += glm::vec2(enemy->spr.Position());
      if (shotRect.origin.x < enemyRect.origin.x + enemyRect.size.x &&
        shotRect.origin.x + shotRect.size.x > enemyRect.origin.x &&
        shotRect.origin.y < enemyRect.origin.y + enemyRect.size.y &&
        shotRect.origin.y + shotRect.size.y > enemyRect.origin.y) {
        shot->health -= 1;
        enemy->health -= 1;
        score += 100;
        break;
      }
    }
  }
  playerBulletList.remove_if([](const Actor& e) { return e.health <= 0; });
  enemyList.remove_if([](const Actor& e) { return e.health <= 0; });
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
  for (auto i = enemyList.begin(); i != enemyList.end(); ++i) {
    renderer.AddVertices(i->spr);
  }
  for (auto i = playerBulletList.begin(); i != playerBulletList.end(); ++i) {
    renderer.AddVertices(i->spr);
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
* 2�̋�`�̏Փˏ�Ԃ𒲂ׂ�.
*
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
