#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

typedef struct Player {
    Vector2 pos;
    Vector2 dir;
    float width;
    float length;
    float moveSpeed;
} Player;

















Vector2 rescale(Vector2 point, const Rectangle MAPSIZE, const Rectangle BORDER) {

    // 1. Subtract back room origin to get local coordinates
    point = Vector2Subtract(point, (Vector2){BORDER.x, BORDER.y + BORDER.height});
    
    // 2. Compute scaling factors
    float scaleX = MAPSIZE.width / BORDER.width;
    float scaleY = MAPSIZE.height / BORDER.height;

    // 3. Apply scaling
    point.x *= scaleX;
    point.y *= scaleY;

    // 4. Flip Y if needed (optional depending on your visual setup)
    point.y *= -1;

    // 5. Translate to front room origin
    point = Vector2Add(point, (Vector2){MAPSIZE.x, MAPSIZE.y});

    

    return point;
}







Vector2 attemptedMovement(Player* player, Vector2* bufferPos) {
    Vector2 move = {0, 0};
    float angle = 0;

    if (IsKeyDown(KEY_W)) move.y++;
    if (IsKeyDown(KEY_S)) move.y--;
    if (IsKeyDown(KEY_A)) move.x--;
    if (IsKeyDown(KEY_D)) move.x++;
    if (IsKeyDown(KEY_O)) angle += PI/2;
    if (IsKeyDown(KEY_P)) angle -= PI/2;

    if (move.x != 0 || move.y != 0) {
        move = Vector2Normalize(move);
        *bufferPos = Vector2Add(player->pos, Vector2Scale(move, player->moveSpeed));
    }

    return Vector2Rotate(move, angle);
}



void movePlayer(Player *player, const Rectangle BORDER) {
    Vector2 bufferPos = player->pos;
    Vector2 bufferDir = attemptedMovement(player, &bufferPos);

    Rectangle futurePlayer = {
        bufferPos.x,
        bufferPos.y,
        player->width,
        player->length
    };

    bool inBounds =
        bufferPos.x >= BORDER.x &&
        bufferPos.x + player->width <= BORDER.x + BORDER.width &&
        bufferPos.y >= BORDER.y &&
        bufferPos.y + player->length <= BORDER.y + BORDER.height;

    if (inBounds) {
        player->pos = bufferPos;
    }
    if (bufferDir.x != 0 || bufferDir.y != 0) {
        player->dir.y = -bufferDir.y;
        player->dir.x = bufferDir.x;
    }
}




void drawWalls3D(const Rectangle WALLS) {
    float wallHeight = 50.0f;
    float wallThickness = 0.2f;
    float y = wallHeight / 2.0f;

    // Bottom wall
    DrawCubeV((Vector3){WALLS.x + WALLS.width/2, y, WALLS.y}, 
              (Vector3){WALLS.width, wallHeight, wallThickness}, RED);

    // Top wall
    DrawCubeV((Vector3){WALLS.x + WALLS.width/2, y, WALLS.y + WALLS.height}, 
              (Vector3){WALLS.width, wallHeight, wallThickness}, BLUE);

    // Left wall
    DrawCubeV((Vector3){WALLS.x, y, WALLS.y + WALLS.height/2}, 
              (Vector3){wallThickness, wallHeight, WALLS.height}, YELLOW);

    // Right wall
    DrawCubeV((Vector3){WALLS.x + WALLS.width, y, WALLS.y + WALLS.height/2}, 
              (Vector3){wallThickness, wallHeight, WALLS.height}, GREEN);
}




void drawMapPlayer(Player player, const Rectangle MAPSIZE, const Rectangle BORDER) {
    Vector2 frontPlayerPos = rescale(player.pos, MAPSIZE, BORDER);

    // Compute scaling factors
    float scaleX = MAPSIZE.width / BORDER.width;
    float scaleY = MAPSIZE.height / BORDER.height;
    // Centering
    frontPlayerPos.x += player.width * scaleX / 2;
    frontPlayerPos.y -= player.length * scaleY / 2;

    DrawCircleV(frontPlayerPos, player.length * scaleY * 0.25, RED);
    DrawLineEx(frontPlayerPos, Vector2Add(frontPlayerPos, Vector2Scale(player.dir, 4)), 4, RED);
}



void drawMap(const Rectangle ROOM) {
    Vector2 topLeft = {ROOM.x, ROOM.y};
    Vector2 topRight = {ROOM.x + ROOM.width, ROOM.y};
    Vector2 bottomRight = {ROOM.x + ROOM.width, ROOM.y + ROOM.height};
    Vector2 bottomLeft = {ROOM.x, ROOM.y + ROOM.height};

    // Draw walls with different colors
    DrawLineEx(topLeft, topRight, 4, RED);        // Top wall
    DrawLineEx(topRight, bottomRight, 4, GREEN);  // Right wall
    DrawLineEx(bottomRight, bottomLeft, 4, BLUE); // Bottom wall
    DrawLineEx(bottomLeft, topLeft, 4, YELLOW);   // Left wall
}


void draw(Player player, const Rectangle MAPSIZE, const Rectangle BORDER) {
    drawMapPlayer(player, MAPSIZE, BORDER);
    drawMap(MAPSIZE);
}





void gameloop() {

    Player player = {
        .pos = {0, 0},
        .dir = {0, -1},
        .width = 4.0f,
        .length = 4.0f,
        .moveSpeed = 0.5f
    };


    
    const Rectangle MAPSIZE = {10, 10, 200, 200};
    const Rectangle BORDER = {-50, -50, 100, 100};
    const Rectangle WALLS = {-200, -200, 400, 400};


    Camera3D camera = {0};
    camera.position = (Vector3){0, 2 ,0};
    camera.target = Vector3Add(camera.position, (Vector3){player.dir.x, 0.0f, player.dir.y});
    camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Y is up
    camera.fovy = 45.0f;                                // Field of view
    camera.projection = CAMERA_PERSPECTIVE;  


    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(GRAY);

        if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
        }

        Vector2 scaled3d = rescale(player.pos, WALLS, BORDER);

        BeginMode3D(camera);  // Begin 3D mode with camera
            camera.position = (Vector3){scaled3d.x, 2, scaled3d.y};
            camera.target = Vector3Add(camera.position, (Vector3){player.dir.x, 0, player.dir.y});
            drawWalls3D(WALLS);
        EndMode3D();

        movePlayer(&player, BORDER);
        draw(player, MAPSIZE, BORDER);

        EndDrawing();
    }
}


int main() {
    const int screenWidth = 1400;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Racer");

    SetTargetFPS(60); // Limit to 60 frames per second
    SetExitKey(KEY_NULL);




    gameloop();

    CloseWindow(); // Clean up
    return 0;
}
