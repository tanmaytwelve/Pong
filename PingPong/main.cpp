#include <iostream>
#include "raylib.h"

/* Colors */
Color Green = { 38, 185, 154, 255 };
Color Dark_Green = { 28, 160, 133, 255 };
Color Light_Green = { 129, 204, 184, 255 };
Color Yellow = { 243, 213, 91, 255 };

//Scores to display 
int playerScore = 0;
int cpuScore = 0;

#pragma region Ball
class Ball
{
public:
	float X, Y; //position of ball
	int Speed_X, Speed_Y; //speed of ball
	int BallRadius; //size of ball

	Sound ResetSound;
	Sound BallHitSound;
	Sound BgSound;

	Ball() //Constructor
	{
		InitAudioDevice();
		
		ResetSound = LoadSound("Sounds/Hurt.mp3");
		BallHitSound = LoadSound("Sounds/Hit.mp3");
	}

	~Ball() //Destructor
	{
		UnloadSound(ResetSound);
		UnloadSound(BallHitSound);
	}

	void Draw() //Drawing a Circle to represent the Ball 
	{
		DrawCircle(X, Y, BallRadius, Yellow);
	}

	void Update() //Updating the speed of Ball by adding speed to Ball's Position
	{
		X += Speed_X;
		Y += Speed_Y;

		if (Y + BallRadius >= GetScreenHeight() || Y - BallRadius <= 0) //Checks if Ball hits to the height of Screen Bounds and Change the Speed of Ball accordingly
		{
			Speed_Y *= -1;
		}

		if (X + BallRadius >= GetScreenWidth())//if Ball goes beyond Player, CPU score gets added and Ball Resets
		{
			cpuScore++;
			ResetBall();
		}
		if (X - BallRadius <= 0) //if Ball goes beyond CPU, Player score gets added and Ball Resets
		{
			playerScore++; 
			ResetBall();
		}
	}
	
	/* Resets Ball position to Center of Screen and adds Speed to Ball*/
	void ResetBall()
	{
		X = GetScreenWidth() / 2;
		Y = GetScreenHeight() / 2;

		PlaySound(ResetSound);

		int speedChoices[2] = { -1, 1 };
		Speed_X *= speedChoices[GetRandomValue(0, 1)];
		Speed_Y *= speedChoices[GetRandomValue(0, 1)];
	}
};
#pragma endregion


#pragma region Paddle
class Paddle
{
public:
	float X, Y; //position of paddle
	float Width, Height; //size of paddle
	int Speed; //speed of paddle

	void Draw() //Drawing Paddle
	{
		DrawRectangleRounded(Rectangle{ X, Y, Width, Height }, 20, 20, WHITE);
	}

	void Update()
	{
		/* Inputs to Handle Paddle Movement */
		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
		{
			Y = Y - Speed;
		}
		if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
		{
			Y = Y + Speed;
		}
		LimitMovement();
	}

protected:
	void LimitMovement() //Limiting Player's Paddle from going out of Screen Bounds
	{
		if (Y <= 0)
		{
			Y = 0;
		}
		if (Y + Height >= GetScreenHeight())
		{
			Y = GetScreenHeight() - Height;
		}
	}

};
#pragma endregion


#pragma region CPU
class CPUPaddle : public Paddle
{
public:
	void Update(int Ball_Y)
	{ 
		/* Moving CPU Paddle to follow Ball's Height */
		if (Y + Height / 2 > Ball_Y)
		{
			Y = Y - Speed;
		}
		if (Y + Height / 2 <= Ball_Y)
		{
			Y = Y + Speed;
		}

		LimitMovement(); //Limiting CPU Paddle Movement from going out of Screen Bounds
	}
};
#pragma endregion



Ball ball;
Paddle player;
CPUPaddle cpu;

int main()
{
	std::cout << "Starting PingPong" << std::endl;
	int screenWidth = 1280;
	int screenHeight = 800;

	//Ball Variables
	ball.BallRadius = 20;
	ball.X = screenWidth / 2;
	ball.Y = screenHeight / 2;
	ball.Speed_X = 7;
	ball.Speed_Y = 7;
	
	//Paddle Var
	player.Width = 25;
	player.Height = 120;
	player.X = screenWidth - player.Width - 10;
	player.Y = screenHeight / 2 - player.Height / 2;
	player.Speed = 6;

	//CPU Var
	cpu.Width = 25;
	cpu.Height = 120;
	cpu.X = 10;
	cpu.Y = screenHeight / 2 - cpu.Height / 2;
	cpu.Speed = 6;

	Music BgMusic = LoadMusicStream("Sounds/bg2.mp3");
	PlayMusicStream(BgMusic);

	InitWindow(screenWidth, screenHeight, "Ping Pong"); //Initializing Window and OpenGL Context
	SetTargetFPS(60);

	while (WindowShouldClose() == false) //looping the window till it closed
	{
		
		BeginDrawing(); //setting up canvas to draw stuffs
		UpdateMusicStream(BgMusic);
		//Updating
		ball.Update();
		player.Update();
		cpu.Update(ball.Y);

		//Checking Collision with Player Paddle 
		if (CheckCollisionCircleRec(Vector2{ ball.X, ball.Y }, ball.BallRadius, Rectangle{ player.X, player.Y, player.Width, player.Height }))
		{
			ball.Speed_X *= -1;
			PlaySound(ball.BallHitSound);
		}

		if (CheckCollisionCircleRec(Vector2{ ball.X, ball.Y }, ball.BallRadius, Rectangle{ cpu.X, cpu.Y, cpu.Width, cpu.Height }))
		{
			ball.Speed_X *= -1;
			PlaySound(ball.BallHitSound);

		}

		//Drawing
		ClearBackground(Dark_Green);
		DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, Green);
		DrawCircle(screenWidth / 2, screenHeight / 2, 100, Light_Green);

		ball.Draw(); //Drawing Ball
		DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE); // Drawing a line dividing two sides	
		player.Draw(); //Drawing Player Paddle
		cpu.Draw(); //Drawing CPU Paddle
		DrawText(TextFormat("%i", cpuScore), screenWidth / 4 - 20, 20, 80, WHITE);
		DrawText(TextFormat("%i", playerScore), 3 * screenWidth / 4 - 20, 20, 80, WHITE);

		EndDrawing(); //end up canvas draw
	}

	UnloadMusicStream(BgMusic);
	CloseAudioDevice();
	CloseWindow(); //Closing Window and unloading OpenGL Context

	return 0;
}