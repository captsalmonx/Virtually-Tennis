#ifndef COURT_H
#define COURT_H

// Load environment assets
bool init_court();
void draw_court();
void update_ball(float delta);
bool checkCollision_ball();
void swingCheck(float power);
void clean_court();

#endif