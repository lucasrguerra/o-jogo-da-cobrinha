#include "raylib.h"

#define MAX_SNAKE_LENGTH 255
#define gameSize 30
#define boardColumns 16
#define boardRows 16
#define boardPositionX 465
#define boardPositionY 120

typedef struct Snake {
	Vector2 position;
	Vector2 speed;
	Vector2 size;
	Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Vector2 size;
    Color color;
} Food;

static const int screenWidth = 1000;
static const int screenHeight = 650;

static int bestScore = 0;
static bool gameWin = false;
static bool gameOver = true;
static int reason = 0;
static bool pause = false;
static int framesCounter = 0;
static bool allowMove = false;

static Snake snake[MAX_SNAKE_LENGTH] = {0};
static int counterTail = 1;
static Food food = { 0 };

static Color rainbowColors[] = {
	(Color){236, 26, 36, 255},
	(Color){246, 148, 28, 255},
	(Color){247, 238, 38, 255},
	(Color){34, 177, 76, 255},
	(Color){123, 227, 1, 255},
	(Color){0, 143, 213, 255},
	(Color){41, 42, 116, 255},
	(Color){103, 46, 146, 255}
};

int main(void) {
	InitWindow(screenWidth, screenHeight, "O Jogo da Cobrinha");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {

		Color firstColor = rainbowColors[0];
		for (int i = 0; i < 8; i++) {
			if ((framesCounter % 5) == 0) {
				if (i == 7){
				rainbowColors[i] = firstColor;
				}
				else {
					rainbowColors[i] = rainbowColors[i + 1];
				}
			};
		};

		BeginDrawing();
		ClearBackground((Color){31, 173, 164, 255});

		DrawBoard();
		DrawTittle();
		UpdateGame();
		UpdateInfos();

		EndDrawing();
		framesCounter++;
	};

	CloseWindow();
	return 0;
}

void InitGame(void) {
	gameWin = false;
	gameOver = false;
	reason = 0;
	pause = false;
	allowMove = true;
	framesCounter = 0;
	counterTail = 1;

	for (int i = 0; i < MAX_SNAKE_LENGTH; i++) {
        snake[i].position = (Vector2){(-2 * gameSize), 0};
        snake[i].size = (Vector2){gameSize, gameSize};
        snake[i].speed = (Vector2){0, 0};
		snake[i].color = (Color){0, 0, 0, 255};

        if (i == 0) {
			snake[i].position = (Vector2){(boardPositionX + gameSize), (boardPositionY + gameSize)};
			snake[i].color = (Color){0, 0, 255, 255};
			snake[i].speed = (Vector2){gameSize, 0};
		};
    };

	food.position = (Vector2){
		(boardPositionX + (gameSize * GetRandomValue(0, (boardColumns - 1)))),
		(boardPositionY + (gameSize * GetRandomValue(0, (boardRows - 1))))
		};
	food.size = (Vector2){gameSize, gameSize};
    food.color = RED;
	return 0;
}

void DrawBoard(void) {
	int boardWidth = boardColumns * gameSize;
	int boardHeigth = boardRows * gameSize;
	DrawRectangle(
		boardPositionX - 4,
		boardPositionY - 4,
		boardWidth + 8,
		boardHeigth + 8,
		(Color){56, 0, 128, 255}
	);
	DrawRectangle(
		boardPositionX,
		boardPositionY, 
		boardWidth,
		boardHeigth,
		WHITE
	);

	for (int i = 0; i < boardColumns + 1; i++) {
		int linePositionXAndHeigth = boardPositionX + i * gameSize;
		DrawLine(
			linePositionXAndHeigth,
			boardPositionY,
			linePositionXAndHeigth,
			boardHeigth + boardPositionY,
			(Color){10, 10, 10, 15}
		);
	};
	for (int i = 0; i < boardRows + 1; i++) {
		int linePositionYAndWidth = boardPositionY + i * gameSize;
		DrawLine(
			boardPositionX,
			linePositionYAndWidth,
			boardWidth + boardPositionX,
			linePositionYAndWidth,
			(Color){10, 10, 10, 15}
		);
	};
	return 0;
}

void DrawTittle(void) {
	DrawText("O Jogo da", 430, 20, 25, BLACK);
	const char *cobrinha[] = {'C', 'o', 'b', 'r', 'i', 'n', 'h', 'a'};
	DrawRainbowText(cobrinha, 368, 50, 50, 8, 33, true, 4, 35);
	return 0;
}

void UpdateGame(void) {
	if (!gameOver && !gameWin) {
		if (IsKeyPressed('P')) {
			pause =	!pause;
		};

		if (!pause) {
			if (allowMove) {
				CheckKeys();
			};
			if ((framesCounter % 8) == 0) {
				UpdateSnake();
			};
		};
		DrawSnake();
		DrawFood();
	}
	else {
		if (IsKeyPressed(KEY_ENTER)) {
			InitGame();
		};
	};
	return 0;
}

void UpdateInfos(void) {
	if (gameOver && bestScore == 0) {
		DrawText("Pressione ENTER", 100, 140, 30, BLACK);
		DrawText("Para iniciar", 145, 170, 30, BLACK);
	}
	else if (gameOver) {
		DrawText("Pressione ENTER", 90, 140, 30, BLACK);
		DrawText("Para reiniciar", 120, 170, 30, BLACK);
		if (reason == 1) {
			DrawText("Não saia do", 135, 230, 30, RED);
			DrawText("tabuleiro!", 149, 260, 30, RED);
		}
		else if (reason == 2) {
			DrawText("Não bata na", 135, 230, 30, RED);
			DrawText("sua calda!", 149, 260, 30, RED);
		};
	}
	else if (gameWin) {
		const char *parabens[] = {'P', 'a', 'r', 'a', 'b', 'e', 'n', 's', '!'};
		DrawRainbowText(parabens, 80, 150, 50, 9, 40, false, 0, 0);
		const char *voce[] = {'v', 'o', 'c', 'e'};
		DrawRainbowText(voce, 160, 190, 50, 4, 40, false, 0, 0);
		const char *venceu[] = {'v', 'e', 'n', 'c', 'e', 'u', '!'};
		DrawRainbowText(venceu, 110, 230, 50, 7, 40, false, 0, 0);
	}
	else {
		DrawText("Tamanho: ", 120, 140, 30, BLACK);
		DrawText(TextFormat("%03i", (counterTail + 1)), 270, 140, 30, BLACK);
		DrawText("Meta: ", 150, 180, 30, BLACK);
		DrawText("256", 240, 180, 30, BLACK);
	};

	if (!pause && !gameOver && !gameWin) {
		DrawText("Aperte P para", 120, 490, 30, DARKGRAY);
		DrawText("pausar o jogo", 125, 520, 30, DARKGRAY);
	}
	else if (pause) {
		DrawText("Aperte P para", 120, 490, 30, DARKGRAY);
		DrawText("continuar o jogo", 105, 520, 30, DARKGRAY);
	};

	DrawText("Maior tamanho: ", 90, 570, 30, GOLD);
	DrawText(TextFormat("%03i", bestScore), 325, 570, 30, GOLD);
	return 0;

}

void CheckKeys(void) {
	if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && (snake[0].speed.y != gameSize)) {
		allowMove = false;
		return snake[0].speed = (Vector2){0, -gameSize};
	};
	if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && (snake[0].speed.y != -gameSize)) {
		allowMove = false;
		return snake[0].speed = (Vector2){0, gameSize};
	};
	if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && (snake[0].speed.x != gameSize)) {
		allowMove = false;
		return snake[0].speed = (Vector2){-gameSize, 0};
	};
	if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && (snake[0].speed.x != -gameSize)) {
		allowMove = false;
		return snake[0].speed = (Vector2){gameSize, 0};
	};
	return 0;
}

void UpdateSnake(void) {
	int newPositionX = snake[0].position.x + snake[0].speed.x;
	int newPositionY = snake[0].position.y + snake[0].speed.y;

	if (
		newPositionX == food.position.x &&
		newPositionY == food.position.y &&
		counterTail < MAX_SNAKE_LENGTH
	) {
		counterTail++;
	};

	if ((
			newPositionX < boardPositionX ||
			newPositionX > boardPositionX + ((boardColumns - 1) * gameSize) ||
			newPositionY < boardPositionY ||
			newPositionY > boardPositionY + ((boardRows - 1) * gameSize)
		)
	) {
		gameOver = true;
		reason = 1;
		if ((counterTail + 1) > bestScore) {
			bestScore = counterTail + 1;
		};
	}
	if (counterTail > 3 && !ThisPositionIsEmpty(newPositionX, newPositionY)) {
		gameOver = true;
		reason = 2;
		if ((counterTail + 1) > bestScore) {
			bestScore = counterTail + 1;
		};
	}
	else {
		UpdateTrail();
		snake[0].position = (Vector2){newPositionX, newPositionY};
		allowMove = true;
	};

	if (newPositionX == food.position.x && newPositionY == food.position.y) {
		if (counterTail < MAX_SNAKE_LENGTH) {
			UpdateFood();
		}
		else {
			gameWin = true;
			bestScore = 256;
		};
	};

	return 0;
}

void UpdateTrail(void) {
	int index = 0;
	for (int i = counterTail; i > 0; i--) {
		snake[i].position = snake[i - 1].position;
		snake[i].speed = snake[i - 1].speed;

		int colorRefence = (255 / counterTail) * index;
		snake[i].color = (Color){0, (255 - colorRefence), colorRefence, 255};
		index += 1;
	};
	return 0;
}

void UpdateFood(void){
	bool isEmpty = false;

	while(!isEmpty) {
		int newFoodPositionX = boardPositionX + (gameSize * GetRandomValue(0, (boardColumns - 1)));
		int newFoodPositionY = boardPositionY + (gameSize * GetRandomValue(0, (boardRows -1)));
		bool check = ThisPositionIsEmpty(newFoodPositionX, newFoodPositionY);
		if (check) {
			isEmpty = true;
			return food.position = (Vector2){newFoodPositionX, newFoodPositionY};
		}
	};
}

int ThisPositionIsEmpty(int positionX, int positionY) {
	for (int i = counterTail; i > 0; i--) {
		if (snake[i].position.x == positionX && snake[i].position.y == positionY) {
			return false;
		};
	};
	return true;
}

void DrawRainbowText(
		char *text[],
		int positionX,
		int positionY,
		int size,
		int sizeOfArray,
		int spacing, 
		bool haveAnI,
		int whereIsI,
		int spacingI
	) {
	for (int i = 0; i < sizeOfArray; i++) {
		if (haveAnI && i == whereIsI) {
			DrawText(&text[i], (positionX + (i * spacingI)), positionY, size, (Color)rainbowColors[i]);
		}
		else {
			DrawText(&text[i], (positionX + (i * spacing)), positionY, size, (Color)rainbowColors[i]);
		};
	};
	return 0;
}

void DrawSnake(void) {
	for (int i = 0; i < MAX_SNAKE_LENGTH; i++) {
		DrawRectangle(
			snake[i].position.x,
			snake[i].position.y,
			snake[i].size.x,
			snake[i].size.y,
			snake[i].color
		);
	};
	return 0;
}

void DrawFood(void) {
	DrawRectangle(
		food.position.x,
		food.position.y,
		food.size.x,
		food.size.y,
		food.color
	);
	return 0;
}