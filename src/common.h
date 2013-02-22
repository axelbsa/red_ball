#ifndef COMMON_H_
#define COMMON_H_ 1

#include <allegro.h>

extern int has_sound;
extern BITMAP *backbuffer;

extern int display_width;
extern int display_height;

void game_init();
void game_tick(double delta_time);
void draw_frame();

#endif /* COMMON_H_ */
