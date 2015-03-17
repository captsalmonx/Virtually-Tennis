#ifndef COURT_H
#define COURT_H

struct tennis_ball {
	int point_count;
	vec3 direction;
	vec3 velocity;
};

// Load environment assets
bool init_court();
void draw_court();
void clean_court();

#endif