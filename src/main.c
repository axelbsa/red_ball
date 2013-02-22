#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#define MIN_INTERVAL (1.0 / 100.0)
#define MAX_LATENCY   0.5

int has_sound = 1;
BITMAP *backbuffer;

int display_width;
int display_height;

void setup_allegro()
{
	display_width = 800 ;
	display_height = 600;

	allegro_init();
	install_timer();
	install_keyboard();
	install_joystick(JOY_TYPE_AUTODETECT);
	install_mouse();

	if (-1 == install_sound(DIGI_AUTODETECT, MIDI_NONE, 0))
	{
		fprintf(stderr, "Disabling sound\n");
		has_sound = 0;
	}

	set_color_depth(32);

	if (-1 == set_gfx_mode(GFX_AUTODETECT_WINDOWED, display_width, display_height, 0, 0))
	{
		fprintf(stderr, "Fatal error: Failed to set graphisc mode %dx%d\n",
                        display_width, display_height);
		allegro_exit();
		exit(EXIT_FAILURE);
	}

	/* Create our canvas */
	backbuffer = create_bitmap(display_width,display_height);
}

double get_time()
{
#ifndef WIN32
	struct timeval tv;

	gettimeofday(&tv, 0);

	return tv.tv_sec + tv.tv_usec * 1.0e-6;
#else
	return GetTickCount() / 1000.0;
#endif
}

int main(int argc, char **argv)
{
	double start_time;
	double current_time;
	double target_time;
	double last_game_time = 0.0;
	int iterations;

	setup_allegro();

	start_time = get_time();
	game_init();
	
	// GAME - LOOP
	while (!key[KEY_ESC])
	{
		current_time = get_time();
		target_time = current_time - start_time;

		/* If the computer's clock has been adjusted backwards,
		 * compensate */
		if (target_time < last_game_time)
			start_time = current_time - last_game_time;

		/* If the game time lags too much, for example if the computer
		 * has been suspended, avoid trying to catch up */
		if (target_time > last_game_time + MAX_LATENCY)
			last_game_time = target_time - MAX_LATENCY;

		/* If more than MIN_INTERVAL has passed since last update, run
		 * game_tick() again. */
		if (target_time >= last_game_time + MIN_INTERVAL)
		{
			game_tick(target_time - last_game_time);
			last_game_time = target_time;
		}

		/* Clear the canvas */
		clear(backbuffer);

		draw_frame();

		/* We are done drawing on the canvas -- show it to the user */
		blit(backbuffer, screen, 0, 0, 0, 0, display_width, display_height);
	}

	allegro_exit();

	return EXIT_SUCCESS;
}
