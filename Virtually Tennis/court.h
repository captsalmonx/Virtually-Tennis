#ifndef COURT_H
#define COURT_H

#include "obj_parser.h"

// Load environment assets
bool init_court(int * score, int * combo);
void draw_court();
bool create_target();
void delete_target(plane * target);
void update_court(float delta);
void static update_targets(float delta);
void static update_ball(float delta);
void static reset_ball();
bool static checkCollision_ball(float delta);
void clean_court();

#endif