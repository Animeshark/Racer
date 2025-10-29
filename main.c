#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

typedef struct Inputs {
    int up[2];
    int left[2];
    int right[2];
    int down[2];
    int enter[2];
} Inputs;


typedef struct Player {
    Vector2 pos; 
    Vector2 velocity; 
    Vector2 direction; // the direction the front wheels are facing
    float acceleration;
    float width;
    float length;
    float breaking;
} Player;










// easier to do this than write the if statement every time
// very easy to just sub these into the if statement if I thinks its better

bool getUp(Inputs hotkeys) {
     return IsKeyPressed(hotkeys.up[0]) || IsKeyPressed(hotkeys.up[1]);
}

bool getDown(Inputs hotkeys) {
    return IsKeyPressed(hotkeys.down[0]) || IsKeyPressed(hotkeys.down[1]);
}

bool getLeft(Inputs hotkeys) {
    return IsKeyPressed(hotkeys.left[0]) || IsKeyPressed(hotkeys.left[1]);
}

bool getRight(Inputs hotkeys) {
    return IsKeyPressed(hotkeys.right[0]) || IsKeyPressed(hotkeys.right[1]);
}

// dif cause enter key must be pressed

bool getEnter(Inputs hotkeys) {
    return IsKeyPressed(hotkeys.enter[0]) || IsKeyPressed(hotkeys.enter[1]);
}


bool checkWindowSize(int *screenWidth, int *screenHeight) {
    // Returns true if window size changes
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }
    if (*screenWidth != GetScreenWidth() || *screenHeight != GetScreenHeight()) {
        *screenWidth = GetScreenWidth();
        *screenHeight = GetScreenHeight();
        return true;
    }
    return false;
}

Vector2 centreText(Vector2 aimedPos, const Font *FONT, const char *text, float size, float spacing) {
    Vector2 dimentions = MeasureTextEx(*FONT, text, size, spacing);
    return Vector2Subtract(aimedPos, Vector2Scale(dimentions, 0.5f));
}













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
        resultantAcceleration = Vector2Subtract(resultantAcceleration, Vector2Scale(player->direction, player->breaking));
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
void updateCameraLookOnly(Camera3D *camera, float sensitivity) {
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

void drawPauseMenu() {
    
}







void gameloop(const int FRAMERATE, unsigned short *gameState) {

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
    


    while (*gameState == 2) {

        movePlayer(&player, TRACK);
        
        if (WindowShouldClose()) *gameState = -1; // will set gameState to bit integer, used for exit
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
        if (IsKeyPressed(KEY_ESCAPE)) *gameState = 1;


        BeginDrawing();
        ClearBackground(GRAY);
    
        BeginMode3D(camera);  // Begin 3D mode with camera
            camera.position = (Vector3){player.pos.x, 2, player.pos.y};
            updateCameraLookOnly(&camera, sensitivity);
            drawWalls3D(TRACK);
        EndMode3D();
        draw(player, MAPSIZE, TRACK);
        EndDrawing();
    }
}



void startMenu(const int FRAMERATE, unsigned short *gameState, int screenWidth, int screenHeight, Inputs hotkeys) {
    
    //Font
    const Font DOOM = LoadFontEx("Assets/font/Amazdoomleft-epw3.ttf", 128, 0, 0);
    SetTextureFilter(DOOM.texture, TEXTURE_FILTER_POINT);

    //Initialising textures
    const Texture2D bg = LoadTexture("Assets/Menu/Backgrounds/Stary.png");
    const Texture2D pointer = LoadTexture("Assets/Menu/pointer.png");

    //text
    const char titleText[] = "SUPER RACEY RACER";
    const char startText[] = "Start";
    const char exitText[] = "Exit";
    const char settingsText[] = "Settings";

    /* Sizes
    They are floats although will only hold integer values. 
    Scaling with integers leads to smooth edges.
    Floats are used to avoid realtime casting in the raylib text func*/

    float adjustmentFactor = roundf((float)screenHeight / 800.0f); //scale with screen size. 1 at base size

    float buttonSize = 36.0f * adjustmentFactor * 2; //Multiplying 36 because I want the resolution to be a clean multi
    float titleSize = 2.0f * buttonSize;
    float spacing = buttonSize / 18.0f;

    // Exit needs dimentions because he is reletive to a side
    Vector2 exitDimentions = MeasureTextEx(DOOM, exitText, buttonSize, spacing);

    // Positions
    Vector2 mid = {screenWidth/2, screenHeight/2};

    Vector2 titlePos = centreText((Vector2){mid.x, mid.y/4}, &DOOM, titleText, titleSize, spacing);
    Vector2 titleShadowPos = {titlePos.x + 5, titlePos.y + 5}; // Offseting shadow
    // Buttons
    Vector2 startPos = centreText((Vector2){mid.x, mid.y}, &DOOM, startText, buttonSize, spacing);
    Vector2 settingsPos = centreText((Vector2){mid.x, screenHeight * 0.75f}, &DOOM, settingsText, buttonSize, spacing);
    Vector2 exitPos = {10, screenHeight - 10 - exitDimentions.y}; // centers it on only the y cordinate
    
    // Pointer 
    Vector2 pointerPos = {startPos.x - 10 - pointer.width, startPos.y}; // starts next to start button




    unsigned short hoveredButton = 0;
    /*
        start = 0 
        settings = 1
        exit = 2
    */




    while(*gameState == 0){

        if (WindowShouldClose()) *gameState = -1; // will set gameState to bit integer limit, used for exit
        if (checkWindowSize(&screenWidth, &screenHeight)) {

            //readjusting size
            adjustmentFactor = (float)screenHeight / 800.0f; 
            buttonSize = 36.0f * adjustmentFactor * 2; 
            titleSize = 2.0f * buttonSize;
            spacing = buttonSize / 18.0f;


            mid = (Vector2){screenWidth/2, screenHeight/2};

            titlePos = centreText((Vector2){mid.x, mid.y/4}, &DOOM, titleText, titleSize, spacing);
            titleShadowPos = (Vector2) {titlePos.x + 5, titlePos.y + 5};

            startPos = centreText((Vector2){mid.x, mid.y}, &DOOM, startText, buttonSize, spacing);
            settingsPos = centreText((Vector2){mid.x, screenHeight* 0.75f}, &DOOM, settingsText, buttonSize, spacing);
            // centers exit on only the y co-ordinate
            exitPos = (Vector2) {10, screenHeight - 10 - exitDimentions.y}; 
        }
        
        // Pointer next to the selected button
        switch (hoveredButton) {
            case 0:
                pointerPos = (Vector2){startPos.x - pointer.width - 10, startPos.y};
                break;
            case 1:
                pointerPos = (Vector2){settingsPos.x - pointer.width - 10, settingsPos.y};
                break;
            case 2:
                pointerPos = (Vector2){exitPos.x + exitDimentions.x + 10, exitPos.y};
                break;
            default:
                printf("%s", "Error with pointer selection in start menu");
                break;
        }

        // Inputhandling

        if (getDown(hotkeys) && hoveredButton != 2) hoveredButton++;
        if (getUp(hotkeys) && hoveredButton != 0) hoveredButton--;

        if (getEnter(hotkeys)) {
            switch (hoveredButton) {
                case 0: *gameState = 2; break; // Start game
                case 1: *gameState = 1; break; // Settings
                case 2: *gameState = -1; break; // Exit
                default: break;
            }
        }

    BeginDrawing();
        ClearBackground(WHITE);
        
        // Background
        DrawTexturePro(
            bg, 
            (Rectangle){0, 0, bg.width, bg.height}, 
            (Rectangle){0, 0, screenWidth, screenHeight}, 
            (Vector2){0, 0}, 0, WHITE
        );
    
        //pointer
        DrawTexture(pointer, pointerPos.x, pointerPos.y, WHITE);

        // Draw title and shadow
        DrawTextEx(DOOM, titleText, titleShadowPos, titleSize, spacing, BLUE);
        DrawTextEx(DOOM, titleText, titlePos, titleSize, spacing, RED);

        //buttons
        DrawTextEx(DOOM, startText, startPos, buttonSize, spacing, RED);
        DrawTextEx(DOOM, settingsText, settingsPos, buttonSize, spacing, RED);
        DrawTextEx(DOOM, exitText, exitPos, buttonSize, spacing, RED);

    EndDrawing();
    }

    // Cleanup
    UnloadTexture(bg);
    UnloadTexture(pointer);
    UnloadFont(DOOM);
}




void settingsMenu(const int FRAMERATE, unsigned short *gameState, int screenWidth, int screenHeight, Inputs *hotkeys){
    
    //Initialising Textures
    const Texture2D bg = LoadTexture("Assets/Menu/Backgrounds/Stary.png");
    const Texture2D pointer = LoadTexture("Assets/Menu/pointer.png");

    const Font DOOM = LoadFontEx("Assets/font/Amazdoomleft-epw3.ttf", 128, 0, 0);
    SetTextureFilter(DOOM.texture, TEXTURE_FILTER_POINT);

    float adjustmentFactor = roundf((float)screenHeight / 800.0f); //scale with screen size. 1 at base size

    float buttonSize = 36.0f * adjustmentFactor * 2;
    float titleSize = 2.0f * buttonSize;
    float spacing = buttonSize / 18.0f;

    // Text 
    const char titeText[] = "Settings";
    
    const char hotkeysText[] = "Hotkeys";
    const char healthText[] = "Health Colour";
    const char musicText[] = "Music Volume:";
    const char gameText[] = "Game Sound Volume:";
    const char menuText[] = "Menu Volume:";
    const char enemyText[] = "Enemy Difficulty:";

    // Dimentions
    Vector2 titleDimentions = MeasureTextEx(DOOM, titeText, titleSize, spacing);

    Vector2 hotkeysDimentions = MeasureTextEx(DOOM, hotkeysText, buttonSize, spacing);
    Vector2 healthDimentions = MeasureTextEx(DOOM, healthText, buttonSize, spacing);
    Vector2 musicDimentions = MeasureTextEx(DOOM, musicText, buttonSize, spacing);
    Vector2 gameDimentions = MeasureTextEx(DOOM, gameText, buttonSize, spacing);
    Vector2 menuDimentions = MeasureTextEx(DOOM, menuText, buttonSize, spacing);
    Vector2 enemyDimentions = MeasureTextEx(DOOM, enemyText, buttonSize, spacing);
    
    // Positions
    Vector2 titlePos = {20, 20}



}




int main() {
    int screenWidth = 1600;
    int screenHeight = 900;
    const int FRAMERATE = 60;

    InitWindow(screenWidth , screenHeight, "Racer");
    SetWindowState(FLAG_WINDOW_RESIZABLE); // Draging the corner will resize the window

    SetTargetFPS(FRAMERATE); // Limit to 60 frames per second
    SetExitKey(KEY_NULL); // Stops pressing esc closing the window

    //Setting default input values
    Inputs hotkeys = {
        .up = {KEY_W, KEY_UP},
        .down = {KEY_S, KEY_DOWN},
        .left = {KEY_A, KEY_LEFT},
        .right = {KEY_D, KEY_RIGHT},
        .enter = {KEY_ENTER, KEY_SPACE}
    };

    // GameState handling

    unsigned short gameState = 0;
    while (gameState >= 0 && gameState <= 3) {
        switch (gameState) {
            case 0:
                startMenu(FRAMERATE, &gameState, screenWidth, screenHeight, hotkeys);
                break;

            case 1:
                // settings

            case 2:
                gameloop(FRAMERATE, &gameState);
                break;

            case 3:
                // loss
            
            default:
                break;
        }
    }






    CloseWindow(); // Clean up
    return 0;
}