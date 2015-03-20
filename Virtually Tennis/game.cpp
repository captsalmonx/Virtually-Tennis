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

#define VERT_SHADER "Shaders/bg.vert"
#define FRAG_SHADER "Shaders/bg.frag"
#define BG_IMAGE "Assets/sky.png"

GLuint bg_sp, bg_texID;

float timer = GAMELENGTH;
int score = 0, combo = 0, comboMax = 0;
int texts[3];

bool * inGamePointer;

char * difficulties[3] = { "Easy", "Medium", "Hard" };
int difficultyIndex = 0;

char * getDifficulty(){
	return difficulties[difficultyIndex];
}

char * cycleDifficulty(){
	difficultyIndex++;
	if(difficultyIndex > sizeof(difficulties) / sizeof(char*) - 1)
		difficultyIndex = 0;

	return difficulties[difficultyIndex];
}

bool gameOver;

bool * getGameOverPointer(){
	return &gameOver;
}

int getScore(){
	return score;
}

int getCombo(){
	return combo;
}

int getMaxCombo(){
	return comboMax;
}

bool checkCombo(){
	if(combo > comboMax){
		comboMax = combo;
		return true;
	}else return false;
}

bool init_game(bool * inGame)
{
	inGamePointer = inGame;

	bg_sp = link_programme_from_files(VERT_SHADER, FRAG_SHADER);
	bg_texID = create_texture_from_file(BG_IMAGE);

	texts[0] = add_text (
		"Score: ", 
		-0.95f, 1.0f, 110.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	texts[1] = add_text (
		"Combo: ",
		-0.95f, 0.9f, 110.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	texts[2] = add_text (
		"Timer:",
		0.4f, 1.0f, 110.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);
	
	return init_player() && init_court(&score, &combo, &difficultyIndex);
}

void reset_game()
{
	timer = GAMELENGTH;
	score = 0;
	combo = 0;
	reset_court();
	reset_player();
}

void update_game(float deltaTime)
{
	update_player(deltaTime);
	update_court(deltaTime);

	timer -= deltaTime;
	if(timer <= 0.0f){
		gameOver = true;
		*inGamePointer = false;
	}

	char tmp[52];
	sprintf(tmp, "Score: %i", score);
	update_text(texts[0], tmp);

	sprintf(tmp, "Combo: %i", combo);
	update_text(texts[1], tmp);

	sprintf(tmp, "Time Left: %i", (int)timer);
	update_text(texts[2], tmp);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(bg_sp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bg_texID);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glClear( GL_DEPTH_BUFFER_BIT );
	draw_court();
	draw_player();

	glClear( GL_DEPTH_BUFFER_BIT );
	for(int i = 0; i < sizeof(texts) / sizeof(int); i++){
		draw_text(texts[i]);
	}
}