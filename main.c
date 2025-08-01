#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

typedef struct Player {
    Vector2 pos; 
    Vector2 velocity; 
    Vector2 direction; // the direction the front wheels are facing
    float acceleration;
    float width;
    float length;
    float breaking;
} Player;















Vector2 rescale(Vector2 point, const Rectangle MAPSIZE, const Rectangle TRACK) {
    Vector2 result;

    float scaleX = MAPSIZE.width / TRACK.width;
    float scaleY = MAPSIZE.height / TRACK.height;

    result.x = MAPSIZE.x + (point.x - TRACK.x) * scaleX;
    result.y = MAPSIZE.y + (point.y - TRACK.y) * scaleY;

    return result;
}




void attemptedMovement(Player *player, Vector2 *bufferPos) {

    Vector2 resultantAcceleration = {0, 0};
    float degrees = 0.0f;

    //Rotation
    if(IsKeyDown(KEY_A)) degrees -= PI/8;
    if(IsKeyDown(KEY_D)) degrees += PI/8;

    //Acceleration
    if(IsKeyDown(KEY_W)) {
        Vector2 accelerationDirection = Vector2Rotate(player->direction, degrees);
        resultantAcceleration = Vector2Add(resultantAcceleration, Vector2Scale(accelerationDirection, player->acceleration));
    }
    //Breaking
    else if(IsKeyDown(KEY_S)) {
        resultantAcceleration = Vector2Subtract(resultantAcceleration, Vector2Scale(player->velocity, player->breaking));
    }
    //Resistanve Force
    resultantAcceleration = Vector2Subtract(resultantAcceleration, Vector2Scale(player->velocity, 0.125));
    player->velocity = Vector2Add(player->velocity, resultantAcceleration);
    if(player->velocity.x != 0 || player->velocity.y != 0){
        player->direction =  Vector2Normalize(player->velocity);
        
    }
    *bufferPos = Vector2Add(*bufferPos, player->velocity);
}

void movePlayer(Player *player, const Rectangle TRACK) {
    Vector2 bufferPos = player->pos;
    attemptedMovement(player, &bufferPos);


    Rectangle futurePlayer = {
        bufferPos.x,
        bufferPos.y,
        player->width,
        player->length
    };

    bool inBounds =
        bufferPos.x >= TRACK.x &&
        bufferPos.x + player->width <= TRACK.x + TRACK.width &&
        bufferPos.y >= TRACK.y &&
        bufferPos.y + player->length <= TRACK.y + TRACK.height;

    if (inBounds) player->pos = bufferPos;


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


void drawMapPlayer(Player player, const Rectangle MAPSIZE, const Rectangle TRACK) {
    // Convert top-left player position to center position in world space
    Vector2 playerCenter = {
        player.pos.x + player.width / 2,
        player.pos.y + player.length / 2
    };

    // Rescale the center position from world space (TRACK) to minimap (MAPSIZE)
    Vector2 minimapPos = rescale(playerCenter, MAPSIZE, TRACK);

    // Draw the player as a small circle
    DrawCircleV(minimapPos, 4, RED);  // Adjust radius as needed

    // Draw the direction vector (e.g., facing forward)
    Vector2 directionEnd = Vector2Add(minimapPos, Vector2Scale(player.direction, 10));
    DrawLineEx(minimapPos, directionEnd, 2, RED);
    
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


void draw(Player player, const Rectangle MAPSIZE, const Rectangle TRACK) {
    drawMapPlayer(player, MAPSIZE, TRACK);
    drawMap(MAPSIZE);
}


// Tbh completely Ai idk how this function works
void UpdateCameraLookOnly(Camera3D *camera, float sensitivity) {
    Vector2 mouseDelta = GetMouseDelta();

    float yaw = -mouseDelta.x * sensitivity;
    float pitch = -mouseDelta.y * sensitivity;

    // Clamp vertical look (pitch) to avoid flipping
    static float totalPitch = 0.0f;
    totalPitch += pitch;
    if (totalPitch > 89.0f) { pitch -= totalPitch - 89.0f; totalPitch = 89.0f; }
    if (totalPitch < -89.0f) { pitch -= totalPitch + 89.0f; totalPitch = -89.0f; }

    // Get current forward vector
    Vector3 forward = Vector3Subtract(camera->target, camera->position);

    // Apply yaw rotation around the up axis
    Matrix yawMat = MatrixRotate(camera->up, DEG2RAD * yaw);
    forward = Vector3Transform(forward, yawMat);

    // Calculate right vector and pitch
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera->up));
    Matrix pitchMat = MatrixRotate(right, DEG2RAD * pitch);
    forward = Vector3Transform(forward, pitchMat);

    // Update target
    camera->target = Vector3Add(camera->position, forward);
}






void gameloop(const int FRAMERATE) {

    Player player = {
        .pos = {800, 800},
        .direction = {0, -1},
        .width = 4.0f,
        .length = 4.0f,
        .acceleration = 13.4/FRAMERATE,
        .breaking = 0.5,
        .velocity = {0, 0}
    };


    
    const Rectangle MAPSIZE = {10, 10, 200, 200};
    const Rectangle TRACK = {0, 0, 1750, 1750};

    float sensitivity = 0.2;


    Camera3D camera = {0};
    camera.position = (Vector3){0, 2 ,0};
    camera.target = Vector3Add(camera.position, (Vector3){player.direction.x, 0.0f, player.direction.y});
    camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Y is up
    camera.fovy = 45.0f;                                // Field of view
    camera.projection = CAMERA_PERSPECTIVE;  


    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(GRAY);

        if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
        }

        BeginMode3D(camera);  // Begin 3D mode with camera
            camera.position = (Vector3){player.pos.x, 2, player.pos.y};
            UpdateCameraLookOnly(&camera, sensitivity);
            drawWalls3D(TRACK);
        EndMode3D();

        movePlayer(&player, TRACK);
        draw(player, MAPSIZE, TRACK);

        EndDrawing();
    }
}


int main() {
    const int SCREENWIDTH = 1400;
    const int SCREENHIEGHT = 800;
    const int FRAMERATE = 60;

    InitWindow(SCREENWIDTH, SCREENHIEGHT, "Racer");

    SetTargetFPS(FRAMERATE); // Limit to 60 frames per second
    SetExitKey(KEY_NULL);




    gameloop(FRAMERATE);

    CloseWindow(); // Clean up
    return 0;
}
