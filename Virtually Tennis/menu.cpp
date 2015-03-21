// Include GLFW
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <string.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

#include "gl_utils.h"
#include "audioPlayer.h"
#include "menu.h"
#include "game.h"
#include "text.h"

#define VERT_SHADER "Shaders/bg.vert"
#define FRAG_SHADER "Shaders/bg.frag"
#define BG_IMAGE "Assets/bg.png"
#define BG_MUSIC "Assets/menu.ogg"

#define FONTCOLOUR vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define HIGHLIGHTCOLOUR vec4(1.0f, 0.2f, 0.2f, 1.0f)

GLuint menu_sp, menu_texID;

int options[4];
int miniText[2];
char * optionText[4] = { "Start Game", "Difficulty", "Level", "Exit Game" };
int optionIndex = 0;

int keyIndex = -1;
const int keys[4] = { GLFW_KEY_ENTER, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_ESCAPE };

bool * ingamePointer;
bool * gameOver;
bool gameOverTextInitialized;

int gameOverText;
int highScore;

bool init_menu(bool * inGame)
{
	ingamePointer = inGame;
	gameOver = getGameOverPointer();

	// Initialize background
	menu_sp = link_programme_from_files(VERT_SHADER, FRAG_SHADER);
	menu_texID = create_texture_from_file(BG_IMAGE);

	// Initialize texts
	options[0] = add_text(
		optionText[0],
		-0.9f, 0.3f, 150.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	optionText[1] = getDifficulty();
	options[1] = add_text(
		optionText[1],
		-0.9f, 0.0f, 150.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);
	miniText[0] = add_text(
		"Difficulty:",
		-0.91f, 0.05f, 80.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	optionText[2] = getLevel();
	options[2] = add_text(
		optionText[2],
		-0.9f, -0.3f, 150.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);
	miniText[1] = add_text(
		"Level:",
		-0.91f, -0.25f, 80.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	options[3] = add_text(
		optionText[3],
		-0.9f, -0.6f, 150.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	gameOverText = add_text(
		"Game Over",
		-0.6f, 0.3f, 110.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	reset_menu();
	return true;
}

void reset_menu()
{
	// Highlight currently selected item
	highlight_option(true);

	// Start bg music
	loopSound(BG_MUSIC);
}

void update_menu()
{
	// If no key has been pressed yet
	if(keyIndex == -1){
		// Check if any of the keys we're looking for has been pressed
		for(int i = 0; i < sizeof(keys) / sizeof(int); i++){
			// If yes that's our currently held key
			if(glfwGetKey( window, keys[i] ) == GLFW_PRESS){
				keyIndex = i;
				break;
			}
		}
	}else{
		// Has the currently held key been released yet?
		if(glfwGetKey( window, keys[keyIndex] ) == GLFW_RELEASE ){
			// If yes we can run this once
			handle_keyPress(keys[keyIndex]);
			keyIndex = -1;
		}
	}

	// Has the game over text been initialized?
	if(*gameOver && !gameOverTextInitialized){
		loopSound(BG_MUSIC);
		char tmp[256];
		sprintf(tmp, "Game, set and match!\nYour score is: %i\nYour highest combo was: %i\nPress enter to return to menu", getScore(), getMaxCombo());
		update_text(gameOverText, tmp);
		gameOverTextInitialized = true;
	}

	// Draw everythang
	draw_menu();
}

void handle_keyPress(int key)
{
	// Handle our key press and release - this happens once per release, and avoids callback functions
	switch(key)	{
		// You've chosen to activate the currently selected option
		case GLFW_KEY_ENTER:
			if(!*gameOver){
				switch(optionIndex){
					// Start game
					case 0:
						stopSounds();
						reset_game();
						*ingamePointer = true;
						gameOverTextInitialized = false;
					break;

					// Change difficulty
					case 1:
						optionText[optionIndex] = cycleDifficulty();
						update_text(options[optionIndex], optionText[optionIndex]);
					break;

					// Change level
					case 2:
						optionText[optionIndex] = cycleLevel();
						update_text(options[optionIndex], optionText[optionIndex]);
					break;

					// Exit
					case 3:
						glfwSetWindowShouldClose (window, 1);
					break;
				};
			}else{
				// Return to main menu
				*gameOver = false;
				optionIndex = 0;
			}
		break;

		// Select the option below
		case GLFW_KEY_DOWN:
			highlight_option(false);

			optionIndex++;
			if(optionIndex > sizeof(options) / sizeof(int) - 1){
				optionIndex = 0;
			}

			highlight_option(true);
		break;

		// Select the option above
		case GLFW_KEY_UP:
			highlight_option(false);

			optionIndex--;
			if(optionIndex < 0){
				optionIndex = sizeof(options) / sizeof(int) - 1;
			}

			highlight_option(true);
		break;
	};
}

void highlight_option(bool highlight)
{
	// Highlight the option by changing it's colour
	change_text_colour(options[optionIndex], highlight ? HIGHLIGHTCOLOUR : FONTCOLOUR);
	update_text(options[optionIndex], optionText[optionIndex]);
}

void draw_menu()
{
	// Everything is being drawn without depth here, so this only need to be cleared once
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw a plane with the menu texture directly to the viewport
	glUseProgram(menu_sp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menu_texID);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Draw texts dependant on menu state
	if(!*gameOver){
		// Draw both options and static texts
		for(int i = 0; i < sizeof(options) / sizeof(int); i++){
			draw_text(options[i]);
		}
		for(int i = 0; i < sizeof(miniText) / sizeof(int); i++){
			draw_text(miniText[i]);
		}
	}else{
		draw_text(gameOverText);
	}
}