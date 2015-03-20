#ifndef GAME_H
#define GAME_H

bool init_game(bool * inGame);
char * getDifficulty();
char * cycleDifficulty();
bool * getGameOverPointer();
int getScore();
int getCombo();
int getMaxCombo();
bool checkCombo();

void update_game(float deltaTime);
void addPoints(int point);
void reset_game();

#endif