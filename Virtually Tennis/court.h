#ifndef COURT_H
#define COURT_H

#include "obj_parser.h"

// Load environment assets
bool init_court(int * score, int * combo, int * difficulty, int * level);
void reset_court();
void draw_court();
bool create_target();
void delete_target(plane * target);
void update_court(float delta);
void static update_targets(float delta);
void static update_ball(float delta);
void static reset_ball();
bool static checkCollision_ball(float delta);
void hitBall(vec3 position, vec3 direction, float power, float swing);
void missBall();
void clean_court();

#endif