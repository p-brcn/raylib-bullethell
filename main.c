#include <raylib.h>
#include <stdio.h>

#include "globals.h"
#include "enemy.h"

#define MAX_BULLETS 100
#define MAX_ENEMIES 100
#define MAX_MISSILES 10
#define MISSILE_SPEED 250

typedef struct {
    Vector2 position;
    float radius;
    float cooldown;
    bool active;
} Bullet;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float speed;
} Player;

typedef struct {
  float explotionRadius;
  float projectileSpeed;
  float cooldown;
  float elapsedTime;
} PlayerWeapon;

Bullet bullets[MAX_BULLETS];

typedef struct {
  Vector2 position;
  float radius;
  float cooldown;
  bool active;
  int health;
  float elapsedTime;
  Vector2 missileTarget;
} Boss;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  float speed;
  bool active;
} Missile;

float dt = 0;
float windowTime = 0;
int screenWidth = 450;
int screenHeight = 800;

unsigned int enemyCount = 0;


Player player;
PlayerWeapon primaryWeapon;

const char *title = "Bullet Hell";

void printDebugInfo(void) {
  printf("%ui\n", enemyCount);
}

void player_not_offscreen() {
    if (player.position.x < 0) {
        player.position.x = 0;
    }
    if (player.position.x > screenWidth) {
        player.position.x = screenWidth;
    }
    if (player.position.y < 0) {
        player.position.y = 0;
    }
    if (player.position.y > screenHeight) {
        player.position.y = screenHeight;
    }
}

Vector2 CalculateMissileVelocity(Vector2 startPos, Vector2 targetPos, float speed) {
    Vector2 direction;
    direction.x = targetPos.x - startPos.x;
    direction.y = targetPos.y - startPos.y;

    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }

    Vector2 velocity;
    velocity.x = direction.x * speed;
    velocity.y = direction.y * speed;

    return velocity;
}

int main() {
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(60);

    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }


    player.position = (Vector2){ screenWidth / 2, screenHeight - 50 };
    player.velocity = (Vector2){ 0, 0 };
    player.speed = 800.0f;
    player.color = WHITE;

    Boss boss;
    boss.position = (Vector2){screenWidth/2, screenHeight/4};
    boss.radius = 20.0f;
    boss.cooldown = 2.0f;
    boss.active = true;
    boss.health = 10;
    boss.elapsedTime = 0.0f;

    primaryWeapon.projectileSpeed = 1000.0f;
    primaryWeapon.cooldown = 0.02f;  // Cooldown time in seconds

    Enemy enemies[MAX_ENEMIES];
    Missile missiles[MAX_MISSILES];
    for (int i = 0; i < MAX_MISSILES; i++) {
        missiles[i].active = false;
    }

        while (!WindowShouldClose()) {
        dt = GetFrameTime();
        windowTime = GetTime();

        // Update
        primaryWeapon.elapsedTime += dt;
        boss.elapsedTime += dt;

        // Player movement
        player.velocity.x = 0;
        player.velocity.y = 0;
        
        if (IsKeyDown(KEY_LEFT)) {
            player.velocity.x = -player.speed * dt;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            player.velocity.x = player.speed * dt;
        }
        if (IsKeyDown(KEY_UP)) {
            player.velocity.y = -player.speed * dt;
        }
        if (IsKeyDown(KEY_DOWN)) {
            player.velocity.y = player.speed * dt;
        }

        // if space is pressed, spawn enemy at random x position y top
        if (IsKeyPressed(KEY_E)) {
            printf("Enemy spawned\n");
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) {
                    enemies[i].position = (Vector2){ GetRandomValue(0, screenWidth), 0 };
                    enemies[i].radius = 10.0f;
                    enemies[i].active = true;
                    enemies[i].health = 1;
                    enemies[i].speed = 100.0f;
                    enemyCount++;
                    break;
                }
            }
        }

        if (boss.active) {
          if (boss.elapsedTime >= boss.cooldown) {
            boss.missileTarget.x = boss.position.x - player.position.x;
            boss.missileTarget.y = boss.position.y - player.position.y;
            boss.elapsedTime = 0.0f;
          }
        }

        player_not_offscreen();

        player.position.x += player.velocity.x;
        player.position.y += player.velocity.y;

        // Update bullets
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].position.y -= primaryWeapon.projectileSpeed * dt;  // ~~~~~~~~ >>> speed >>> 

                // offscreen?
                if (bullets[i].position.y + bullets[i].radius < 0) {
                    bullets[i].active = false;
                }
            }
        }

        // Update enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active) {
                UpdateEnemy(&enemies[i], dt);

                if (enemies[i].position.y > screenHeight || enemies[i].health <= 0) {
                    enemies[i].active = false;
                }
            }
        }

        // Spawn bullets with cooldown
        if (IsKeyDown(KEY_SPACE) && primaryWeapon.elapsedTime >= primaryWeapon.cooldown) {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) {
                    bullets[i].position = (Vector2){ player.position.x, player.position.y - 20 };
                    bullets[i].radius = 5.0f;
                    bullets[i].active = true;

                    bullets[i].position.x += player.velocity.x * 2;

                    primaryWeapon.elapsedTime = 0.0f;  // Reset the elapsed time
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_MISSILES; i++) {
          if (missiles[i].active) {
            missiles[i].position.x += missiles[i].velocity.x * dt;
            missiles[i].position.y += missiles[i].velocity.y * dt;

            if (missiles[i].position.y > screenHeight) {
              missiles[i].active = false;
            }
          }
        }

        for (int i = 0; i < MAX_MISSILES; i++) {
          if (!missiles[i].active) {
            missiles[i].position = boss.position;
            missiles[i].velocity = CalculateMissileVelocity(missiles[i].position, boss.missileTarget, MISSILE_SPEED);
            missiles[i].active = true;
          } 
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw player
        DrawCircleV(player.position, 10, player.color);

        // Draw enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active) {
                enemies[i].hit = false;
                DrawEnemy(&enemies[i], RED);
            }
        }

        // Draw bullets
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                DrawCircleV(bullets[i].position, bullets[i].radius, RED);
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (enemies[j].active) {
                        if (CheckCollisionCircles(bullets[i].position, bullets[i].radius, enemies[j].position, enemies[j].radius)) {
                            enemies[j].health -= 1;
                            enemies[j].hit = true;
                            bullets[i].active = false;
                            break;
                        }
                    }
                }
            }
        }

        // Draw boss
        if (boss.active) {
          DrawCircleV(boss.position, boss.radius, BLUE);
        }

        // Draw boss missiles
        for (int i = 0; i < MAX_MISSILES; i++) {
          if (missiles[i].active) {
            DrawCircleV(missiles[i].position, 5, WHITE);
          }
        }

        // Draw FPS
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 10, WHITE);
        DrawText(TextFormat("S: %f", windowTime), 10, 20, 10, WHITE);
        // Enemies on screen
        DrawText(TextFormat("Enemies: %u", enemyCount), 10, 30, 10, WHITE);

        //printDebugInfo();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
