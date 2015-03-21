// Include GLFW
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

#include "game.h"
#include "court.h"
#include "player.h"
#include "text.h"
#include "audioPlayer.h"

#define GAMELENGTH 61.0f;

#define VERT_SHADER "Shaders/bg.vert"
#define FRAG_SHADER "Shaders/bg.frag"
#define BG_IMAGE "Assets/sky.png"

const char* bg_textures[3] = {"Assets/sky_1.png", "Assets/sky_2.png", "Assets/sky_3.png"};
int bg_texIDs[3];
GLuint bg_sp;

char * difficulties[3] = { "Easy", "Medium", "Hard" };
int difficultyIndex = 0;

char * levels[3] = { "Sky", "Ocean", "Hell" };
char * levelMusic[3] = { "Assets/level_1.ogg", "Assets/level_2.ogg", "Assets/level_3.ogg" };
int levelIndex = 0;

float timer = GAMELENGTH;
int score = 0, combo = 0, comboMax = 0;
int texts[3];
bool gameOver;
bool * inGamePointer;

char * getDifficulty(){
	return difficulties[difficultyIndex];
}

char * cycleDifficulty(){
	difficultyIndex++;
	if(difficultyIndex > sizeof(difficulties) / sizeof(char*) - 1)
		difficultyIndex = 0;

	return difficulties[difficultyIndex];
}

char * getLevel(){
	return levels[levelIndex];
}

char * cycleLevel(){
	levelIndex++;
	if(levelIndex > sizeof(levels) / sizeof(char*) - 1)
		levelIndex = 0;

	return levels[levelIndex];
}

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

	// Create the shader for the background
	bg_sp = link_programme_from_files(VERT_SHADER, FRAG_SHADER);

	// Initialize textures
	for(int i = 0; i < 3; i++){
		bg_texIDs[i] = create_texture_from_file(bg_textures[i]);
	}

	// Initialize texts
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
	
	// Return true if both player and court have initialized okay
	return init_player() && init_court(&score, &combo, &difficultyIndex, &levelIndex);
}

void reset_game()
{
	loopSound(levelMusic[levelIndex]);
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
	// If time runs out it's game over and kicked back to main menu
	if(timer <= 0.0f){
		stopSounds();
		gameOver = true;
		*inGamePointer = false;
	}

	// Update on screen strings
	char tmp[52];
	sprintf(tmp, "Score: %i", score);
	update_text(texts[0], tmp);

	sprintf(tmp, "Combo: %i", combo);
	update_text(texts[1], tmp);

	sprintf(tmp, "Time Left: %i", (int)timer);
	update_text(texts[2], tmp);

	// Clear buffers before drawing background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(bg_sp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bg_texIDs[levelIndex]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Clear buffer so the court and player are drawn on top
	glClear( GL_DEPTH_BUFFER_BIT );
	draw_court();
	draw_player();

	// Draw text on top of everything
	glClear( GL_DEPTH_BUFFER_BIT );
	for(int i = 0; i < sizeof(texts) / sizeof(int); i++){
		draw_text(texts[i]);
	}
}