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

typedef struct Data {

	// this is for my settings menu to be able to access it all in one parameter

	Color healthbarColour;
	float musicVolume;
	float menuVolume;
	float gameVolume;
	unsigned short enemyDifficulty;
	/*
	Key:
	0 = easy 
	1 = normal
	2 = hard
	3 = extreme
	4 = custom
	*/
} Data;

typedef struct Player {
	Vector2 pos; 
	Vector2 velocity; 
	Vector2 direction; // the direction the front wheels are facing
	float acceleration;
	float width;
	float length;
	float breaking;
} Player;

typedef struct FontData {
	const Font *FONT;
	float size;
	float spacing;
} FontData;


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

bool getEnter(Inputs hotkeys) {
	return IsKeyPressed(hotkeys.enter[0]) || IsKeyPressed(hotkeys.enter[1]);
}


bool checkWindowSize(int *screenWidth, int *screenHeight) {
	// Persistent state across calls
	static bool isBorderless = false;
	static unsigned short windowedWidth = 0;
	static unsigned short windowedHeigth = 0;
	static Vector2 windowedPos = {0};

	int currentWidth = GetScreenWidth();
	int currentHeight = GetScreenHeight();

	// Toggle borderless fullscreen on F11
	if (IsKeyPressed(KEY_F11)) {
		if (!isBorderless) {
			// Save current windowed settings
			windowedPos = GetWindowPosition();
			windowedWidth = currentWidth;
			windowedHeigth = currentHeight;

			int monitor = GetCurrentMonitor();
			SetWindowState(FLAG_WINDOW_UNDECORATED);
			SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
			SetWindowPosition(GetMonitorPosition(monitor).x, GetMonitorPosition(monitor).y);

			isBorderless = true;
		} 
		else {
			// Exit borderless fullscreen
			ClearWindowState(FLAG_WINDOW_UNDECORATED);
			SetWindowSize(windowedWidth, windowedHeigth);

			// Returns the window to its origonal pos
			SetWindowPosition((int)windowedPos.x, (int)windowedPos.y);

			isBorderless = false;
		}
	}

	// Detect and update screen size changes


	if (currentWidth != *screenWidth || currentHeight != *screenHeight) {
		*screenWidth = currentWidth;
		*screenHeight = currentHeight;
		return true;
	}
	return false;
}


Vector2 centreText(Vector2 aimedPos, FontData font, const char *text) {
	Vector2 dimentions = MeasureTextEx(*font.FONT, text, font.size, font.spacing);
	return Vector2Subtract(aimedPos, Vector2Scale(dimentions, 0.5f));
}

Vector2 ScaleVector2(Vector2 v, float scaleX, float scaleY) {
	return (Vector2){v.x * scaleX, v.y * scaleY};
}

Vector2 ValuePos(Vector2 lablePos, FontData font, const char *lableText) {
	Vector2 lableDimentions = MeasureTextEx(*font.FONT, lableText, font.size, font.spacing);
	return (Vector2){lablePos.x + lableDimentions.x + 10, lablePos.y};
}

void VolumeToString(char *text, float volume) {
	int percent = (int)(volume * 100.0f + 0.5f);
	sprintf(text, "%d", percent);
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



void startMenu(const int FRAMERATE, unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs hotkeys) {
	
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

	float baseButtonSize = 36.0f * 2; //Multiplying 36 because I want the resolution to be a clean multi
	float baseTitleSize = 2.0f * baseButtonSize;
	float baseSpacing = baseButtonSize / 18.0f;

	FontData button = {
		.FONT = &DOOM,
		.size = baseButtonSize,
		.spacing = baseSpacing
	};

	FontData title = {
		.FONT = &DOOM,
		.size = baseTitleSize,
		.spacing = baseSpacing
	};
	
	// Exit needs dimentions because he is reletive to a side
	Vector2 baseExitDimentions = MeasureTextEx(DOOM, exitText, baseButtonSize, baseSpacing);

	// Positions
	Vector2 mid = {SCREENWIDTH/2, SCREENHEIGHT/2};

	Vector2 baseTitlePos = centreText((Vector2){mid.x, mid.y/4}, title, titleText);
	Vector2 baseTitleShadowPos = {baseTitlePos.x + 5, baseTitlePos.y + 5}; // Offseting shadow

	// Buttons
	Vector2 baseStartPos = centreText((Vector2){mid.x, mid.y}, button, startText);
	Vector2 baseSettingsPos = centreText((Vector2){mid.x, SCREENHEIGHT * 0.75f}, button, settingsText);
	Vector2 baseExitPos = {10, SCREENHEIGHT - 10 - baseExitDimentions.y}; // centers it on only the y cordinate

	
	Vector2 exitDimentions = baseExitDimentions;
	Vector2 titlePos = baseTitlePos;
	Vector2 titleShadowPos = baseTitleShadowPos;

	Vector2 startPos = baseStartPos;
	Vector2 settingsPos = baseSettingsPos;
	Vector2 exitPos = baseExitPos;
	
	int buttonSize = baseButtonSize;
	int titleSize = baseTitleSize;
	int spacing = baseSpacing;

	// Pointer
	Vector2 pointerPos = {startPos.x - 10 - pointer.width, startPos.y}; // starts next to start button

	int curWidth = SCREENWIDTH;
	int curHeight = SCREENHEIGHT;

	unsigned short hoveredButton = 0;
	/*
		start = 0 
		settings = 1
		exit = 2
	*/

	while(*gameState == 0){

		if (WindowShouldClose()) *gameState = -1; // will set gameState to bit integer limit, used for exit

		if (checkWindowSize(&curWidth, &curHeight)) {

			//readjusting size
			float scaleX = (float) curWidth / (float) SCREENWIDTH;
			float scaleY = (float) curHeight / (float) SCREENHEIGHT;

			buttonSize = baseButtonSize * scaleY;
			titleSize = baseTitleSize * scaleY;
			spacing = baseSpacing * scaleY;

			titlePos = ScaleVector2(baseTitlePos, scaleX, scaleY);
			titleShadowPos = ScaleVector2(baseTitleShadowPos, scaleX, scaleY);
			startPos = ScaleVector2(baseStartPos, scaleX, scaleY);
			settingsPos = ScaleVector2(baseSettingsPos, scaleX, scaleY);
			exitPos = ScaleVector2(baseExitPos, scaleX, scaleY);
			exitDimentions = ScaleVector2(baseExitDimentions, scaleX, scaleY);
		}
		
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
				case 0: *gameState = 3; break; // Start game
				case 1: *gameState = 2; break; // Settings
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
			(Rectangle){0, 0, curWidth, curHeight}, 
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



void settingsMenu(const int FRAMERATE, unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs *hotkeys, Data *info){
	
	//Initialising Textures
	const Texture2D bg = LoadTexture("Assets/Menu/Backgrounds/Stary.png");
	const Texture2D pointer = LoadTexture("Assets/Menu/pointer.png");

	const Font DOOM = LoadFontEx("Assets/font/Amazdoomleft-epw3.ttf", 128, 0, 0);
	SetTextureFilter(DOOM.texture, TEXTURE_FILTER_POINT);

	// Base Sizes
	float baseButtonSize = 36.0f * 2;
	float baseTitleSize = 2.0f * baseButtonSize;
	float baseSpacing = baseButtonSize / 18.0f;
	
	FontData doomFont = {
		.FONT = &DOOM,
		.size = baseButtonSize,
		.spacing = baseSpacing
	};

	// Text 
	const char titeText[] = "Settings";
	
	const char hotkeyText[] = "Hotkeys";
	const char healthText[] = "Health Colour";
	const char musicText[] = "Music Volume:";
	const char gameText[] = "Game Sound Volume:";
	const char menuText[] = "Menu Volume:";
	const char enemyText[] = "Enemy Difficulty:";
	const char backText[] = "Back";
	
	// Volumes
	char musicValueText[5];
	char gameValueText[5];
	char menuValueText[8];

	VolumeToString(musicValueText, info->musicVolume);
	VolumeToString(gameValueText, info->gameVolume);
	VolumeToString(menuValueText, info->menuVolume);
	
	// Base Positions
	int baseLeftmargin = pointer.width + 20;
	int baseButtonSpacing = 20;

	// Persistant images
	Vector2 baseTitlePos = {40, 40};
	Vector2 baseHotkeyPos = {baseLeftmargin, baseTitlePos.y + baseTitleSize + 40};
	Vector2 baseHealthPos = {baseLeftmargin, baseHotkeyPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseMusicPos = {baseLeftmargin, baseHealthPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseGamePos = {baseLeftmargin, baseMusicPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseMenuPos = {baseLeftmargin, baseGamePos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseEnemyPos = {baseLeftmargin, baseMenuPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseBackPos = {baseLeftmargin, SCREENHEIGHT - baseButtonSize - 10};


	// Values position
	
	Vector2 baseMusicValuePos = ValuePos(baseMusicPos, doomFont, musicText);
	Vector2 baseGameValuePos = ValuePos(baseGamePos, doomFont, gameText);
	Vector2 baseMenuValuePos = ValuePos(baseMenuPos, doomFont, menuText);

	// Dynamic Positions

	Vector2 titlePos = baseTitlePos;
	Vector2 hotkeyPos = baseHotkeyPos;
	Vector2 healthPos = baseHealthPos;
	Vector2 musicPos = baseMusicPos;
	Vector2 gamePos = baseGamePos;
	Vector2 menuPos = baseMenuPos;
	Vector2 enemyPos = baseEnemyPos;
	Vector2 backPos = baseBackPos;

	Vector2 musicValuePos = baseMusicValuePos;
	Vector2 gameValuePos = baseGameValuePos;
	Vector2 menuValuePos = baseMenuValuePos;

	int buttonSize = baseButtonSize;
	int titleSize = baseTitleSize;
	int spacing = baseSpacing;
	int leftmargin = baseLeftmargin;
	int buttonSpacing = baseButtonSpacing;

	// Pointer
	Vector2 *pointerPositions[] = {
	&hotkeyPos,
	&healthPos,
	&musicPos,
	&gamePos,
	&menuPos,
	&enemyPos,
	&backPos
	};

	// Position arrays

	Vector2 basePosArray[] = {
		baseTitlePos,
		baseHotkeyPos,
		baseHealthPos,
		baseMusicPos,
		baseGamePos,
		baseMenuPos,
		baseEnemyPos,
		baseBackPos,
	};

	Vector2 *posArray[] = {
		&titlePos,
		&hotkeyPos,
		&healthPos,
		&musicPos,
		&gamePos,
		&menuPos,
		&enemyPos,
		&backPos,
	};


	int posArrayLen = sizeof(basePosArray)/sizeof(basePosArray[0]);


	int pointerOffset = 10 + pointer.width;

	Vector2 pointerPos;

	int curWidth = SCREENWIDTH;
	int curHeight = SCREENHEIGHT;

	unsigned short hoveredButton = 0;
	unsigned short selectedButton = 9;
	
	/*
	Key:
		hotkey = 0
		health = 1
		music = 2
		game = 3
		menu = 4
		enemy = 5
		back = 6

		None = 9
		when no button is selected it shows 9.
	*/

	while(*gameState == 2){
		if (WindowShouldClose()) *gameState = -1;
		if(checkWindowSize(&curWidth, &curHeight)){

			float scaleX = (float) curWidth / (float) SCREENWIDTH;
			float scaleY = (float) curHeight / (float) SCREENHEIGHT;

			buttonSize = baseButtonSize * scaleY;
			titleSize = baseTitleSize * scaleY;
			spacing = baseSpacing * scaleY;
			buttonSpacing = baseButtonSpacing * scaleY;

			doomFont.size = buttonSize;
			doomFont.spacing = spacing;

			for (int i = 0; i <= posArrayLen -1; i++) {
				*posArray[i] = ScaleVector2(basePosArray[i], scaleX, scaleY);
			}

			musicValuePos = ValuePos(musicPos, doomFont, musicText);
			gameValuePos = ValuePos(gamePos, doomFont, gameText);
			menuValuePos = ValuePos(menuPos, doomFont, menuText);
		}

		switch (selectedButton)
		{
		case 9:
			pointerPos = (Vector2){pointerPositions[hoveredButton]->x - pointerOffset, pointerPositions[hoveredButton]->y};

			if (getDown(*hotkeys) && hoveredButton != 6) hoveredButton++;
			if (getUp(*hotkeys) && hoveredButton != 0) hoveredButton--;

			if (getEnter(*hotkeys)) selectedButton = hoveredButton;
			break;
		case 1:



			break;    

		case 6:
			*gameState = 0;
			break;  
			
		default:
			break;
		}
	BeginDrawing();
		ClearBackground(WHITE);

				// Background
		DrawTexturePro(
			bg, 
			(Rectangle){0, 0, bg.width, bg.height}, 
			(Rectangle){0, 0, curWidth, curHeight}, 
			(Vector2){0, 0}, 0, WHITE
		);
	
		//pointer
		DrawTexture(pointer, pointerPos.x, pointerPos.y, WHITE);

		// Draw settings title
		DrawTextEx(DOOM, titeText, titlePos, titleSize, spacing, RED);

		// Draw setting options
		DrawTextEx(DOOM, hotkeyText, hotkeyPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, healthText, healthPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, musicText, musicPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, gameText, gamePos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, menuText, menuPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, enemyText, enemyPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, backText, backPos, buttonSize, spacing, RED);

		// Draw values for scrollable settings
		DrawTextEx(DOOM, musicValueText, musicValuePos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, gameValueText, gameValuePos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, menuValueText, menuValuePos, buttonSize, spacing, RED);

	EndDrawing();
	}
	UnloadTexture(bg);
	UnloadTexture(pointer);
	UnloadFont(DOOM);
}




int main() {
	const int SCREENWIDTH = 1600;
	const int SCREENHEIGHT = 900;
	const int FRAMERATE = 60;

	InitWindow((int) SCREENWIDTH, (int) SCREENHEIGHT, "Racer");
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

	//Setting default data values
	Data info = {
		.enemyDifficulty = 1,
		.gameVolume = 0.5f,
		.menuVolume = 0.5f,
		.musicVolume = 0.5f,
		.healthbarColour = RED
	};

	// GameState handling

	unsigned short gameState = 0;
	while (gameState >= 0 && gameState <= 3) {
		switch (gameState) {
			case 0:
				startMenu(FRAMERATE, &gameState, SCREENWIDTH, SCREENHEIGHT, hotkeys);
				break;

			case 1:
				// Car customization
				break;

			case 2:
				settingsMenu(FRAMERATE, &gameState, SCREENWIDTH, SCREENHEIGHT, &hotkeys, &info);

			case 3:
				gameloop(FRAMERATE, &gameState);
				break;

			case 4:
				// loss
			
			default:
				break;
		}
	}


	CloseWindow(); // Clean up
	return 0;
}