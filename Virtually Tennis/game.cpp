// Include GLFW
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

#include "game.h"
#include "court.h"
#include "player.h"
#include "text.h"

#define GAMELENGTH 61.0f;

float timer = GAMELENGTH;
int score = 0, combo = 0;
int scoreText, timerText, comboText;

bool init_game()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	scoreText = add_text (
		"Score: ", 
		-0.95f, 1.0f, 110.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	comboText = add_text (
		"Combo: ",
		-0.95f, 0.9f, 110.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	timerText = add_text (
		"Timer",
		0.4f, 1.0f, 110.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);
	
	return init_player() && init_court(&score, &combo);
}

void reset_game()
{
	timer = GAMELENGTH;
}

void update_game(float deltaTime)
{
	update_player(deltaTime);
	update_court(deltaTime);

	timer -= deltaTime;

	char tmp[52];
	sprintf(tmp, "Time Left: %i", (int)timer);
	update_text(timerText, tmp);

	sprintf(tmp, "Score: %i", score);
	update_text(scoreText, tmp);

	sprintf(tmp, "Combo: %i", combo);
	update_text(comboText, tmp);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	draw_court();
	draw_texts();
}