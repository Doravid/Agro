#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#define MAX_PROJECTILES 512
typedef struct
{
    Vector2 position, size, movementVector;
    Color color;
    float rotation, moveSpeed, rotationSpeed, attackSpeed, attackCooldown;
    uint32_t maxHealth, currentHealth;
    float dashTimer;
} Player;
typedef enum
{
    PlayerProj,
    EnemyProj,
} ProjectileOwner;
typedef struct
{
    Vector2 position, direction;
    Color color;
    float moveSpeed, size;
    uint32_t damage;
    ProjectileOwner owner;
} Projectile;

Projectile projectiles[MAX_PROJECTILES];
uint32_t numProjectiles = 0;

Player enemies[MAX_PROJECTILES];
uint32_t numEnemies = 0;

Player mainPlayer = {
    .maxHealth = 100,
    .currentHealth = 100,
    .position = {400.0f, 200.0f},
    .size = {40.0f, 40.0f},
    .rotation = -90.f,
    .rotationSpeed = 90.f,
    .moveSpeed = 400.0f,
    .color = GREEN,
    .movementVector = (Vector2){0},
    .dashTimer = 0.0f,
};

void drawPlayer(Player player)
{
    const float fadeAmount = 0.3f;
    // Draw the Barrel
    Vector2 childOffset = {30.0f, 0.0f};
    Vector2 childSize = {20.0f, 15.0f};

    Vector2 rotatedOffset = Vector2Rotate(childOffset, player.rotation * DEG2RAD);
    Vector2 childWorldPos = Vector2Add(player.position, rotatedOffset);

    Color barrelDrawColor = player.dashTimer <= 0.f ? DARKPURPLE : Fade(DARKPURPLE, fadeAmount);
    Rectangle childRec = {childWorldPos.x, childWorldPos.y, childSize.x, childSize.y};
    Vector2 childOrigin = {childSize.x * 0.5f, childSize.y * 0.5f};
    DrawRectanglePro(childRec, childOrigin, player.rotation, barrelDrawColor);

    // Draw the Player Cube
    Color playerDrawColor = player.dashTimer <= 0.f ? player.color : Fade(player.color, fadeAmount);

    Rectangle playerRec = {player.position.x, player.position.y, player.size.x, player.size.y};
    Vector2 playerOrigin = {player.size.x * 0.5f, player.size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, player.rotation, playerDrawColor);

    // Draw the Health Bar
    const float healthBarStart = (player.position.x - player.size.x / 4);
    const float healthBarMaxWidth = player.size.x * 1.5;
    Rectangle healthRec = {
        .x = healthBarStart,
        .y = player.position.y + 49.,
        .width = healthBarMaxWidth * ((float)player.currentHealth / player.maxHealth),
        .height = player.size.y / 10.0,
    };
    Rectangle redHealthRec = {
        .x = healthBarStart + healthRec.width,
        .y = player.position.y + 49.,
        .width = healthBarMaxWidth - healthBarMaxWidth * ((float)player.currentHealth / player.maxHealth),
        .height = player.size.y / 10.0,
    };
    DrawRectanglePro(healthRec, playerOrigin, 0, player.color);
    DrawRectanglePro(redHealthRec, playerOrigin, 0, RED);
}

void drawEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        drawPlayer(enemies[enemyIndex]);
    }
}
void spawnProjectile(Projectile proj_to_spawn)
{
    if (numProjectiles < MAX_PROJECTILES)
    {
        projectiles[numProjectiles] = proj_to_spawn;
        numProjectiles++;
    }
}
void spawnProjectileFromPlayer(Player parent, ProjectileOwner owner)
{
    Vector2 directionVector = {.x = cosf(parent.rotation * DEG2RAD), .y = sinf(parent.rotation * DEG2RAD)};
    spawnProjectile((Projectile){
        .color = PINK,
        .direction = directionVector,
        .position = {parent.position.x + directionVector.x * 40, parent.position.y + directionVector.y * 40},
        .size = 10.f,
        .moveSpeed = 250.f,
        .damage = 15,
        .owner = owner,
    });
}

void drawProjectiles(Projectile *projs, uint32_t numProjs)
{
    for (uint16_t projectileIndex = 0; projectileIndex < numProjs; projectileIndex++)
    {
        Projectile currentProjectile = projs[projectileIndex];
        DrawCircle(currentProjectile.position.x, currentProjectile.position.y, currentProjectile.size, currentProjectile.color);
    }
}
void damageEnemy(uint32_t enemyIndex, uint32_t damage)
{
    if (enemies[enemyIndex].currentHealth <= damage)
    {
        enemies[enemyIndex].currentHealth = 0;
        enemies[enemyIndex] = enemies[numEnemies - 1];
        numEnemies--;
    }
    else
        enemies[enemyIndex].currentHealth -= damage;
}
void damagePlayer(uint32_t damage)
{

    mainPlayer.currentHealth -= damage;
}
bool projectileHitsEntity(Projectile proj)
{
    if (proj.owner == EnemyProj && Vector2Distance(proj.position, mainPlayer.position) < proj.size + mainPlayer.size.x / 2)
    {
        if (mainPlayer.dashTimer > 0)
            return false;
        damagePlayer(proj.damage);
        return true;
    }
    if (proj.owner == PlayerProj)
    {
        for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
        {
            if (Vector2Distance(proj.position, enemies[enemyIndex].position) < proj.size + enemies[enemyIndex].size.x / 2)
            {
                damageEnemy(enemyIndex, proj.damage);
                return true;
            }
        }
    }

    return false;
}

void updateProjectiles()
{
    for (uint16_t projectileIndex = 0; projectileIndex < numProjectiles; projectileIndex++)
    {
        Projectile *currentProjectile = &projectiles[projectileIndex];

        currentProjectile->position.x += currentProjectile->direction.x * currentProjectile->moveSpeed * GetFrameTime();
        currentProjectile->position.y += currentProjectile->direction.y * currentProjectile->moveSpeed * GetFrameTime();

        if (projectileHitsEntity(*currentProjectile) || Vector2Distance(currentProjectile->position, mainPlayer.position) > 1000.)
        {
            *currentProjectile = projectiles[numProjectiles - 1];
            numProjectiles--;
            projectileIndex--;
        }
    }
}
void dashPlayer(Player *player)
{
    if (player->dashTimer > 0.f || Vector2LengthSqr(player->movementVector) <= 0)
        return;
    player->dashTimer = 0.15f;
}
void updateEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        Player *currentEnemy = &enemies[enemyIndex];
        if (currentEnemy->attackCooldown <= 0)
        {
            spawnProjectileFromPlayer(*currentEnemy, EnemyProj);
            currentEnemy->attackCooldown += currentEnemy->attackSpeed;
        }
        currentEnemy->attackCooldown -= GetFrameTime();

        float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
        float enemyAngle = currentEnemy->rotation * DEG2RAD;
        float delta = enemyAngle - ang;
        delta = atan2f(sinf(delta), cosf(delta));

        if (delta < 0)
            currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
        else
            currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();
    }
}

// Base virtual design resolution
const float VIRTUAL_WIDTH = 1920.0f;
const float VIRTUAL_HEIGHT = 1080.0f;

void updatePlayer()
{
    // Input & Movement
    if (IsKeyDown(KEY_D))
        mainPlayer.movementVector.x = 1;
    else if (IsKeyDown(KEY_A))
        mainPlayer.movementVector.x = -1;
    else
        mainPlayer.movementVector.x = 0;
    if (IsKeyDown(KEY_W))
        mainPlayer.movementVector.y = -1;
    else if (IsKeyDown(KEY_S))
        mainPlayer.movementVector.y = 1;
    else
        mainPlayer.movementVector.y = 0;

    if (IsKeyDown(KEY_Q))
        mainPlayer.rotation -= mainPlayer.rotationSpeed * GetFrameTime();
    if (IsKeyDown(KEY_E))
        mainPlayer.rotation += mainPlayer.rotationSpeed * GetFrameTime();
    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        spawnProjectileFromPlayer(mainPlayer, PlayerProj);
    if (IsKeyPressed(KEY_LEFT_SHIFT))
        dashPlayer(&mainPlayer);

    if (mainPlayer.dashTimer > 0.f)
    {
        mainPlayer.dashTimer -= GetFrameTime();

        float currentDashSpeed = mainPlayer.moveSpeed * 3.5f;
        Vector2 offset = Vector2Scale(mainPlayer.movementVector, currentDashSpeed * GetFrameTime());
        mainPlayer.position = Vector2Add(mainPlayer.position, offset);
        return;
    }

    Vector2 norm = Vector2Normalize(mainPlayer.movementVector);
    Vector2 offset = Vector2Scale(norm, GetFrameTime() * mainPlayer.moveSpeed);
    mainPlayer.position = Vector2Add(mainPlayer.position, offset);
}

int main(void)
{
    // Init
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1920, 1080, "Fixed FOV Resolution Scaling");
    SetTargetFPS(0);

    // User-controlled zoom multiplier
    float userZoom = 1.0f;

    Camera2D camera = {0};
    enemies[0] = (Player){.rotationSpeed = 50.f, .attackCooldown = 0., .attackSpeed = 0.2, .color = BLUE, .moveSpeed = 400.f, .position = {100, 50}, .rotation = 0, .size = {40.f, 40.f}, .maxHealth = 100, .currentHealth = 100};
    numEnemies = 1;

    Shader bloom = LoadShader(0, "bloom.fs");
    int sizeLoc = GetShaderLocation(bloom, "size");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(bloom, sizeLoc, resolution, SHADER_UNIFORM_VEC2);
    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    SetTextureWrap(target.texture, TEXTURE_WRAP_CLAMP);

    // Main Game Loop
    while (!WindowShouldClose())
    {
        // Fullscreen Toggle
        if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)))
        {
            int currentMonitor = GetCurrentMonitor();
            if (IsWindowFullscreen())
            {
                ToggleFullscreen();
                SetWindowSize(1280, 720);
            }
            else
            {
                SetWindowSize(GetMonitorWidth(currentMonitor), GetMonitorHeight(currentMonitor));
                ToggleFullscreen();
            }
        }
        if (IsWindowResized())
        {
            // Update the render texture.
            UnloadRenderTexture(target);
            target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
            SetTextureWrap(target.texture, TEXTURE_WRAP_CLAMP);
            // Update the shader.
            float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
            SetShaderValue(bloom, sizeLoc, resolution, SHADER_UNIFORM_VEC2);
        }
        updatePlayer();
        // Modify base user zoom
        userZoom = expf(logf(userZoom) + ((float)GetMouseWheelMove() * 0.1f));
        if (userZoom > 3.0f)
            userZoom = 3.0f;
        if (userZoom < 0.1f)
            userZoom = 0.1f;

        // FOV Scaling
        float scaleX = (float)GetScreenWidth() / VIRTUAL_WIDTH;
        float scaleY = (float)GetScreenHeight() / VIRTUAL_HEIGHT;
        float windowScale = fminf(scaleX, scaleY);
        camera.zoom = userZoom * windowScale;
        camera.offset = (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
        camera.target = mainPlayer.position;
        camera.rotation = 0.0f;

        // Movement Updates
        updateProjectiles();

        updateEnemies();

        // Render

        BeginTextureMode(target); // Enable drawing to texture

        ClearBackground(BLACK);

        BeginMode2D(camera);
        drawProjectiles(projectiles, numProjectiles);

        drawPlayer(mainPlayer);
        drawEnemies();

        EndMode2D();
        EndTextureMode();

        // Draw The Game
        BeginDrawing();
        BeginShaderMode(bloom);
        DrawTextureRec(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Vector2){0, 0}, WHITE);
        EndShaderMode();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}