#ifndef PLAYER_H
#define PLAYER_H

#include "obj_parser.h"

bool init_player();
void reset_player();
void update_player(float delta);
void draw_player();
void setBall(object * ballPointer);
void setDimensions(const vec2 * dimPointer);
bool isBallOnScreen();
bool isBallNearPlayer();
mat4 getViewMatrix();
mat4 getProjectionMatrix();

#endif