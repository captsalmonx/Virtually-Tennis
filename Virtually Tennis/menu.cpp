// Include GLFW
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <string.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

#include "gl_utils.h"
#include "menu.h"
#include "game.h"
#include "text.h"

#define VERT_SHADER "Shaders/bg.vert"
#define FRAG_SHADER "Shaders/bg.frag"
#define BG_IMAGE "Assets/bg.png"

#define FONTCOLOUR vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define HIGHLIGHTCOLOUR vec4(1.0f, 0.2f, 0.2f, 1.0f)

GLuint menu_sp, menu_texID;

int options[3];
char * optionText[3] = { "Start Game", "Difficulty: ", "Exit Game" };
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

	menu_sp = link_programme_from_files(VERT_SHADER, FRAG_SHADER);
	menu_texID = create_texture_from_file(BG_IMAGE);

	options[0] = add_text(
		optionText[0],
		-0.9f, 0.1f, 150.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	optionText[1] = getDifficulty();
	options[1] = add_text(
		optionText[1] ,
		-0.9f, -0.2f, 150.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	options[2] = add_text(
		optionText[2],
		-0.9f, -0.5f, 150.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	gameOverText = add_text(
		"Game Over",
		-0.6f, 0.3f, 110.0f,
		1.0f, 1.0f, 1.0f, 1.0f
		);

	highlight_option(true);

	return true;
}

void update_menu()
{
	if(keyIndex == -1){
		for(int i = 0; i < sizeof(keys) / sizeof(int); i++){
			if(glfwGetKey( window, keys[i] ) == GLFW_PRESS){
				keyIndex = i;
				break;
			}
		}
	}else{
		if(glfwGetKey( window, keys[keyIndex] ) == GLFW_RELEASE ){
			handle_keyPress(keys[keyIndex]);
			keyIndex = -1;
		}
	}

	if(*gameOver && !gameOverTextInitialized){
		char tmp[256];
		sprintf(tmp, "Game, set and match!\nYour score is: %i\nYour highest combo was: %i\nPress enter to return to menu", getScore(), getMaxCombo());
		update_text(gameOverText, tmp);
		gameOverTextInitialized = true;
	}
	draw_menu();
}

void handle_keyPress(int key)
{
	switch(key)	{
		case GLFW_KEY_ENTER:
			if(!*gameOver){
				switch(optionIndex){
					// Start game
					case 0:
						reset_game();
						*ingamePointer = true;
						gameOverTextInitialized = false;
					break;

					// Change difficulty
					case 1:
						optionText[optionIndex] = cycleDifficulty();
						update_text(options[optionIndex], optionText[optionIndex]);
					break;

					// Exit
					case 2:
						glfwSetWindowShouldClose (window, 1);
					break;
				};
			}else{
				*gameOver = false;
				optionIndex = 0;
			}
		break;

		case GLFW_KEY_DOWN:
			highlight_option(false);

			optionIndex++;
			if(optionIndex > sizeof(options) / sizeof(int) - 1){
				optionIndex = 0;
			}

			highlight_option(true);
		break;

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
	change_text_colour(options[optionIndex], highlight ? HIGHLIGHTCOLOUR : FONTCOLOUR);
	update_text(options[optionIndex], optionText[optionIndex]);
}

void draw_menu()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(menu_sp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menu_texID);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if(!*gameOver){
		for(int i = 0; i < sizeof(options) / sizeof(int); i++){
			draw_text(options[i]);
		}
	}else{
		draw_text(gameOverText);
	}
}