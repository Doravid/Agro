#include <stdio.h>
#include "enemies.h"
#include "camera.h"
#include "levelManager.h"

Enemy enemies[MAX_PROJECTILES];
uint32_t numEnemies = 0;

void damageEnemy(uint32_t enemyIndex, uint32_t damage)
{
    printf("Current health: %u, damage: %u\n", enemies[enemyIndex].currentHealth, damage);
    if (enemies[enemyIndex].currentHealth <= damage)
    {
        enemies[enemyIndex].currentHealth = 0;
        enemies[enemyIndex] = enemies[numEnemies - 1];
        numEnemies--;
    }
    else
        enemies[enemyIndex].currentHealth -= damage;
    printf("Current health: %u, damage: %ud\n", enemies[enemyIndex].currentHealth, damage);
}

static void drawMeleeEnemy(Enemy *enemy)
{
    // Draw the Player Cube
    Color playerDrawColor = enemy->color;

    Rectangle playerRec = {enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y};
    Vector2 playerOrigin = {enemy->size.x * 0.5f, enemy->size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, enemy->rotation, playerDrawColor);

    // Draw the Health12 Bar
    drawHealthBar(enemy->size, enemy->position, (float)enemy->currentHealth / enemy->maxHealth, enemy->color);
}

static void drawShooter(Enemy *enemy)
{
    drawPlayer(*(Player *)enemy);
}

void drawBuffshot(Enemy enemy)
{
    Vector2 childOffset = {30.0f, 0.0f};
    Vector2 childSize = {17.0f, 30.0f};

    Vector2 rotatedOffset = Vector2Rotate(childOffset, enemy.rotation * DEG2RAD);
    Vector2 childWorldPos = Vector2Add(enemy.position, rotatedOffset);

    Color barrelDrawColor = DARKPURPLE;
    Rectangle childRec = {childWorldPos.x, childWorldPos.y, childSize.x, childSize.y};
    Vector2 childOrigin = {childSize.x * 0.5f, childSize.y * 0.5f};
    DrawRectanglePro(childRec, childOrigin, enemy.rotation, barrelDrawColor);

    Rectangle playerRec = {enemy.position.x, enemy.position.y, enemy.size.x, enemy.size.y};
    Vector2 playerOrigin = {enemy.size.x * 0.5f, enemy.size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, enemy.rotation, enemy.color);

    drawHealthBar(enemy.size, enemy.position, (float)enemy.currentHealth / enemy.maxHealth, enemy.color);
}

void drawBoss1(Enemy *enemy)
{
    // Draw the main body
    DrawCircle(enemy->position.x, enemy->position.y, enemy->size.x * 0.5f, RED);

    // Draw the eyes.
    const int eyeRadius = 8;
    const int leftEyeOffset = enemy->position.x - enemy->size.x / 6;
    const int rightEyeOffset = enemy->position.x + enemy->size.x / 6;
    DrawCircle(leftEyeOffset, enemy->position.y - enemy->size.x / 5, eyeRadius, BLACK);
    DrawCircle(rightEyeOffset, enemy->position.y - enemy->size.x / 5, eyeRadius, BLACK);

    DrawRectangle(leftEyeOffset - eyeRadius / 2, enemy->position.y - 10, 4, 15, BLACK);
    DrawRectangle(rightEyeOffset, enemy->position.y - 10, 4, 15, BLACK);

    Vector2 leftShoulder = Vector2Add(enemy->position, Vector2Rotate((Vector2){-enemy->size.x / 2, 0.0f}, (enemy->rotation) * DEG2RAD));
    Vector2 rightShoulder = Vector2Add(enemy->position, Vector2Rotate((Vector2){enemy->size.x / 2, 0.0f}, (enemy->rotation) * DEG2RAD));

    float arm1Length = 90.0f;
    float arm2Length = 90.0f;

    Vector2 shoulders[2] = {leftShoulder, rightShoulder};
    Vector2 targets[2] = {enemy->extraData.boss.leftArmTargetPoint, enemy->extraData.boss.rightArmTargetPoint};
    const int jointSize = 12;

    for (int i = 0; i < 2; i++)
    {
        float dist = Vector2Distance(shoulders[i], targets[i]);
        if (dist >= arm1Length + arm2Length)
            dist = arm1Length + arm2Length - 0.1f;

        float angleB = acosf((dist * dist + arm1Length * arm1Length - arm2Length * arm2Length) / (2.0f * dist * arm1Length));
        float angleC = acosf((arm1Length * arm1Length + arm2Length * arm2Length - dist * dist) / (2.0f * arm1Length * arm2Length));
        float angleD = atan2f(targets[i].y - shoulders[i].y, targets[i].x - shoulders[i].x);

        float a1 = angleD - (i == 0 ? angleB : -angleB);
        float a2 = a1 + (i == 0 ? PI - angleC : -(PI - angleC));

        Vector2 elbow = {shoulders[i].x + cosf(a1) * arm1Length, shoulders[i].y + sinf(a1) * arm1Length};
        Vector2 hand = {elbow.x + cosf(a2) * arm2Length, elbow.y + sinf(a2) * arm2Length};

        // Draw The Arms
        DrawRectanglePro((Rectangle){shoulders[i].x, shoulders[i].y, arm1Length, jointSize}, (Vector2){0, 5}, a1 * RAD2DEG, GRAY);
        DrawRectanglePro((Rectangle){elbow.x, elbow.y, arm2Length, jointSize}, (Vector2){0, 5}, a2 * RAD2DEG, GRAY);

        // Draw The Joints
        DrawCircleV(shoulders[i], jointSize, DARKGRAY);
        DrawCircleV(elbow, jointSize, DARKGRAY);

        const int handSizeX = 48;
        const int HandSizeX = 30;
        // Draw The Hands
        DrawRectanglePro((Rectangle){hand.x, hand.y, HandSizeX, handSizeX}, (Vector2){HandSizeX / 2, handSizeX / 2}, a2 * RAD2DEG, DARKGRAY);
    }

    if (enemy->extraData.boss.state == BOSS_LASER_WINDUP)
    {
        DrawRectanglePro((Rectangle){enemy->position.x, enemy->position.y, 1000, 20}, (Vector2){0, 10}, enemy->rotation, Fade(RED, 0.3f));
    }
    else if (enemy->extraData.boss.state == BOSS_LASER_FIRE)
    {
        DrawRectanglePro((Rectangle){enemy->position.x, enemy->position.y, 1000, 20}, (Vector2){0, 10}, enemy->rotation, RED);
    }

    drawHealthBar(enemy->size, enemy->position, (float)enemy->currentHealth / enemy->maxHealth, GREEN);
}

void drawEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        switch (enemies[enemyIndex].type)
        {
        case ENEMY_SHOOTER:
            drawShooter(&enemies[enemyIndex]);
            break;
        case ENEMY_MELEE:
            drawMeleeEnemy(&enemies[enemyIndex]);
            break;
        case ENEMY_BUFFSHOT:
            drawBuffshot(enemies[enemyIndex]);
            break;

        case ENEMY_BOSS1:
            drawBoss1(&enemies[enemyIndex]);
            break;
        default:
            break;
        }
    }
}
static void spawnEnemy(float rotationSpeed, float attackSpeed, uint32_t maxHealth, float moveSpeed, Color color, Vector2 position, EnemyType type, uint32_t attackDamage)
{
    enemies[numEnemies] = (Enemy){
        .rotationSpeed = rotationSpeed,
        .attackCooldown = 0.,
        .attackSpeed = attackSpeed,
        .color = color,
        .moveSpeed = moveSpeed,
        .position = position,
        .rotation = 0,
        .size = {40.f, 40.f},
        .maxHealth = maxHealth,
        .currentHealth = maxHealth,
        .type = type,
        .attackDamage = attackDamage,
    };
    numEnemies++;
}

void spawnShooterPos(uint32_t difficulty, Vector2 spawnPos)
{
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 100.;
    uint32_t baseAttackDamage = 15;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, DARKBLUE, (float)randValue / 100), spawnPos, ENEMY_SHOOTER, baseAttackDamage);
}
void spawnMeleePos(uint32_t difficulty, Vector2 spawnPos)
{
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 200.;
    uint32_t baseAttackDamage = 15;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, PINK, (float)randValue / 100), spawnPos, ENEMY_MELEE, baseAttackDamage);
}

void spawnBuffshotPos(uint32_t difficulty, Vector2 spawnPos)
{
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 5.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 14.;
    uint32_t baseAttackDamage = 40;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, PINK, (float)randValue / 100), spawnPos, ENEMY_BUFFSHOT, baseAttackDamage);
}

void spawnBoss1Pos(Vector2 spawnPos)
{

    enemies[numEnemies] = (Enemy){
        .rotationSpeed = 0.00001,
        .attackCooldown = 0.,
        .attackSpeed = 0.5,
        .color = GREEN,
        .moveSpeed = 3,
        .position = spawnPos,
        .rotation = 0,
        .size = {80.f, 80.f},
        .maxHealth = 1000,
        .currentHealth = 1000,
        .type = ENEMY_BOSS1,
        .attackDamage = 75,
    };
    numEnemies++;
}

void spawnRandomEnemyPos(uint32_t difficulty, Vector2 spawnPos)
{
    spawnBoss1Pos(spawnPos);
    return;
    int randValue = GetRandomValue(ENEMY_SHOOTER, ENEMY_BUFFSHOT);
    switch (randValue)
    {
    case ENEMY_SHOOTER:
        spawnShooterPos(difficulty, spawnPos);
        break;
    case ENEMY_MELEE:
        spawnMeleePos(difficulty, spawnPos);
        break;
    case ENEMY_BUFFSHOT:
        spawnBuffshotPos(difficulty, spawnPos);
        break;
    default:
        break;
    }
}

void updateShooter(Enemy *currentEnemy)
{
    // Shoot at the player if possible.
    if (currentEnemy->attackCooldown <= 0)
    {
        spawnProjectileFromPlayer(*(Player *)currentEnemy, EnemyProj);
        currentEnemy->attackCooldown += currentEnemy->attackSpeed;
    }
    currentEnemy->attackCooldown -= GetFrameTime();

    // Rotate Towards the player
    float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
    float enemyAngle = currentEnemy->rotation * DEG2RAD;
    float delta = enemyAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));

    if (delta < 0)
        currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
    else
        currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();
    // Move towards the player (well aktually a point that is 150 units away from the player in the direction of the player.)
    Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(mainPlayer.position, currentEnemy->position));
    Vector2 offset = Vector2Scale(dirToPlayer, 150.0f);
    Vector2 targetPoint = Vector2Subtract(mainPlayer.position, offset);

    currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
    Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
    currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);
}
void updateMelee(Enemy *currentEnemy)
{
    currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();

    Vector2 targetPoint = mainPlayer.position;

    currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
    Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
    currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);

    if (Vector2Distance(targetPoint, currentEnemy->position) < 15.f)
    {
        Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(currentEnemy->position, mainPlayer.position));
        currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, Vector2Scale(dirToPlayer, 90.0f));
        damagePlayer(currentEnemy->maxHealth / 10);
    }
}

void updateBuffshot(Enemy *currentEnemy)
{
    // Shoot at the player if possible.
    if (currentEnemy->attackCooldown <= 0)
    {
        spawnProjectileFromPlayerPro(*(Player *)currentEnemy, EnemyProj, 20.f, 100.f);
        currentEnemy->attackCooldown += currentEnemy->attackSpeed;
    }
    currentEnemy->attackCooldown -= GetFrameTime();

    // Rotate Towards the player
    float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
    float enemyAngle = currentEnemy->rotation * DEG2RAD;
    float delta = enemyAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));

    if (delta < 0)
        currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
    else
        currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();

    // Move towards the player (well aktually a point that is 150 units away from the player in the direction of the player.)
    Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(mainPlayer.position, currentEnemy->position));
    Vector2 offset = Vector2Scale(dirToPlayer, 150.0f);
    Vector2 targetPoint = Vector2Subtract(mainPlayer.position, offset);

    currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
    Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
    currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);
}

void updateBoss1(Enemy *currentEnemy)
{
    currentEnemy->extraData.boss.stateTimer -= GetFrameTime();
    if (currentEnemy->extraData.boss.stateTimer <= 0)
    {
        switch (currentEnemy->extraData.boss.state)
        {
        case BOSS_IDLE:
            currentEnemy->extraData.boss.state = BOSS_SPIN_WINDUP;
            currentEnemy->extraData.boss.stateTimer = 1.0f;
            break;
        case BOSS_SPIN_WINDUP:
            currentEnemy->extraData.boss.state = BOSS_SPIN;
            currentEnemy->extraData.boss.stateTimer = 3.0f;
            break;
        case BOSS_SPIN:
            currentEnemy->extraData.boss.state = BOSS_LASER_WINDUP;
            currentEnemy->extraData.boss.stateTimer = 1.5f;
            break;
        case BOSS_LASER_WINDUP:
            currentEnemy->extraData.boss.state = BOSS_LASER_FIRE;
            currentEnemy->extraData.boss.stateTimer = 1.0f;
            break;
        case BOSS_LASER_FIRE:
            currentEnemy->extraData.boss.state = BOSS_IDLE;
            currentEnemy->extraData.boss.stateTimer = 2.0f;
            break;
        }
    }

    Vector2 leftShoulder = Vector2Add(currentEnemy->position, Vector2Rotate((Vector2){-30.0f, 0.0f}, currentEnemy->rotation * DEG2RAD));
    Vector2 rightShoulder = Vector2Add(currentEnemy->position, Vector2Rotate((Vector2){30.0f, 0.0f}, currentEnemy->rotation * DEG2RAD));

    float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
    float enemyAngle = currentEnemy->rotation * DEG2RAD;
    float delta = enemyAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));

    if (currentEnemy->extraData.boss.state == BOSS_IDLE)
    {
        // Shoot at the player if possible.
        if (currentEnemy->attackCooldown <= 0)
        {
            spawnProjectileFromPlayerPro(*(Player *)currentEnemy, EnemyProj, 20.f, 100.f);
            currentEnemy->attackCooldown += currentEnemy->attackSpeed;
        }
        currentEnemy->attackCooldown -= GetFrameTime();

        // Rotate Towards the player
        if (delta < 0)
            currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
        else
            currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();

        // Move towards the player (well aktually a point that is 150 units away from the player in the direction of the player.)
        Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(mainPlayer.position, currentEnemy->position));
        Vector2 offset = Vector2Scale(dirToPlayer, 150.0f);
        Vector2 targetPoint = Vector2Subtract(mainPlayer.position, offset);

        currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
        Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
        currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);

        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){0.0f, 40.0f}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = Vector2Add(rightShoulder, Vector2Rotate((Vector2){0.0f, 40.0f}, currentEnemy->rotation * DEG2RAD));
    }
    else if (currentEnemy->extraData.boss.state == BOSS_SPIN_WINDUP)
    {
        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){20.0f, 0.0f}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = Vector2Add(rightShoulder, Vector2Rotate((Vector2){-20.0f, 0.0f}, currentEnemy->rotation * DEG2RAD));
    }
    else if (currentEnemy->extraData.boss.state == BOSS_SPIN)
    {
        printf("rotation = %f\n", currentEnemy->rotation);
        currentEnemy->rotation += 600.0f * GetFrameTime();
        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){-currentEnemy->size.x * 3, -21.0f}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = Vector2Add(rightShoulder, Vector2Rotate((Vector2){currentEnemy->size.x * 3, -21.0f}, currentEnemy->rotation * DEG2RAD));
    }
    else if (currentEnemy->extraData.boss.state == BOSS_LASER_WINDUP)
    {
        if (delta < 0)
            currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
        else
            currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();

        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){40.0f, 40.0f}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = Vector2Add(rightShoulder, Vector2Rotate((Vector2){40.0f, -40.0f}, currentEnemy->rotation * DEG2RAD));
    }
    else if (currentEnemy->extraData.boss.state == BOSS_LASER_FIRE)
    {
        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){70.0f, 20.0f}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = Vector2Add(rightShoulder, Vector2Rotate((Vector2){70.0f, -20.0f}, currentEnemy->rotation * DEG2RAD));
    }
}

void updateEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        Enemy *currentEnemy = &enemies[enemyIndex];
        switch (currentEnemy->type)
        {
        case ENEMY_SHOOTER:
            updateShooter(currentEnemy);
            break;
        case ENEMY_MELEE:
            updateMelee(currentEnemy);
            break;
        case ENEMY_BUFFSHOT:
            updateBuffshot(currentEnemy);
            break;

        case ENEMY_BOSS1:
            updateBoss1(currentEnemy);
            break;
        default:
            break;
        }

        // Rotate towards the player
    }
}