#include "enemy.h"
#include "globals.h"
#include <raylib.h>

void UpdateEnemy(Enemy *enemy, float dt) {
    enemy->position.y += enemy->speed * dt;
    if (enemy->position.y > GetScreenHeight() + enemy->radius) {
        enemy->active = false;
    }
    // if health <= 0 then enemy is dead
    if (enemy->health <= 0) {
        enemy->active = false;
        enemyCount--;
    }
}

void DrawEnemy(const Enemy *enemy, Color color) {
    // if enemy hit == true then draw white circle
    // else draw red circle
    if (enemy->hit) {
        DrawCircleV(enemy->position, enemy->radius, WHITE);
    } else {
        DrawCircleV(enemy->position, enemy->radius, color);
    }
}
