#ifndef PLAYER_H
#define PLAYER_H

void init_player();
void update_player(float delta);
void setBall(dynamicObject * ballPointer);
bool swingCheck();
mat4 getViewMatrix();
mat4 getProjectionMatrix();

#endif