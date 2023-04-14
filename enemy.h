#include <raylib.h>
#include <stdbool.h>

#ifndef ENEMY_H
#define ENEMY_H

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float speed;
    float health;
    float radius;
    bool hit;
    bool active;
} Enemy;

void UpdateEnemy(Enemy *enemy, float dt);
void DrawEnemy(const Enemy *enemy, const Color color);

#endif
