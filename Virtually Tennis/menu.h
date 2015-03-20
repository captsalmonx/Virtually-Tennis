#ifndef MENU_H
#define MENU_H

bool init_menu(bool * inGame);
void update_menu();
void handle_keyPress(int key);
void highlight_option(bool highlight);
void draw_menu();

#endif