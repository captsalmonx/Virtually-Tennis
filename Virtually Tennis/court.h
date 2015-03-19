#ifndef COURT_H
#define COURT_H



// Load environment assets
bool init_court();
void draw_court();
bool create_target();
void update_ball(float delta);
bool checkCollision_ball(float delta);
void clean_court();

#endif