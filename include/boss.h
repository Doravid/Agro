#pragma once
typedef enum
{
    BOSS_IDLE,
    BOSS_SPIN_WINDUP,
    BOSS_SPIN,
    BOSS_LASER_WINDUP,
    BOSS_LASER_FIRE
} BossState;

void spawnBoss1Pos(Vector2 spawnPos);