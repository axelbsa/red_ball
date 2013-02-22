#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "common.h"
#include "values.h"

#define WHITE 0xffffff

static const float game_abs_velocity = -109.0f;
static const float ball_abs_velocity = 170.0f;

static const int paddle_height = 48;
static const int paddle0_x = 10;
static const int paddle1_x = 800 - 20;

static float paddle0_y;
static float paddle1_y;

static int brick_x = 32;
static int brick_y = 32;

int jump = 0;
int ground = 0;
int counter = 0;

static float ball_x;
static float ball_y;
static float ball_velocity_x;
static float ball_velocity_y;

static const int ball_width = 32 - 2;
static BITMAP *bg_color;
static BITMAP *brick;
static BITMAP *red_ball;
static BITMAP *stoppers;

//static BITMAP *cube;
//static SAMPLE *bounce;

static char DATADIR[200];

BITMAP *load_tga_or_die(const char *path);
SAMPLE *load_wav_or_die(const char *path);
void play_sound(SAMPLE * sound);

struct stoppers_struct{
    float stopper_x;
    float stopper_y;
    float stopper_vel_x;
    float stopper_vel_y;
    int drew;
}; 

struct stoppers_struct stopps[1000];

static void ball_init()
{
    int i;
    ball_x = values[1];
    ball_y = display_height * 0.2f;
    ball_velocity_x = 0.0f;
    ball_velocity_y = 0.0f;

    for (i=0; i<1000; ++i){ 
        stopps[i].stopper_x = values[i];
        stopps[i].stopper_y = display_width * 0.7f;
        stopps[i].stopper_vel_x = 0.0f;
        stopps[i].stopper_vel_y = 0.0f;
        stopps[i].drew = 0;
    }
    // 4 platforms start values
    for (i=0; i<5; ++i){
        stopps[i].drew = 1;
        counter++;
    }
    
    stopps[0].stopper_y = display_width * 0.4f;
    stopps[1].stopper_y = display_width * 0.3f;
    stopps[2].stopper_y = display_width * 0.6f;
    stopps[3].stopper_y = display_width * 0.1f;
}

void game_init()
{
    int color = 0x23456;
    color = makecol(000, 000, 000);

    bg_color = create_bitmap(display_width, display_height);

    getcwd(DATADIR, sizeof(DATADIR));
    paddle0_y = display_height / 2 - paddle_height / 2;
    paddle1_y = display_height / 2 - paddle_height / 2;

    brick = load_tga_or_die("data/brick.tga");
    red_ball = load_tga_or_die("data/aqua_read-ball.tga");
    stoppers = load_tga_or_die("data/stoppers.tga");
    //bounce = load_wav_or_die("bounce.wav");

    ball_init();
}

/**
 * This function updates the position of all the objects in the scene.
 */
void game_tick(double delta_time)
{
    float next_x, next_y;
    float platform_x, platform_y[5];
    float range_top, range_bottom;
    float angle;
    int i = 0;
    int end = 0;

    ground = 0;

    if (key[KEY_SPACE] /*&& !ball_velocity_x */ ) {
        for (i=counter-4; i<counter; ++i){
	    stopps[i].stopper_vel_y = game_abs_velocity;
        }
    }

    if (key[KEY_LEFT])
	ball_velocity_x -= 10;

    if (key[KEY_RIGHT])
	ball_velocity_x += 10;

    if ((key[KEY_LSHIFT] || key[KEY_UP]) && (!jump) ) {
        //fprintf(stderr, "Trying to jump");
	ball_velocity_y = -550;
	jump = 1;
    }
    
    if (!ground) 
        ball_velocity_y += 981.0f * delta_time;
    
    // Stopper Moving
    for (i=counter-4; i<counter; ++i){
        //platform_x = stopps[i].stopper_x + stopps[i].stopper_vel_x * delta_time;
        platform_y[i] = stopps[i].stopper_y + stopps[i].stopper_vel_y * delta_time;
    }


    // Ball Moving
    next_x = ball_x + ball_velocity_x * delta_time;
    next_y = ball_y + ball_velocity_y * delta_time;


    // Bounce against top of screen? 
    if (next_y < display_height) {
	//ball_velocity_y = -ball_velocity_y;
	//next_y = ball_width / 2;
    } else if (next_y > display_height - ball_width / 2){       // ... or bottom of screen? 
	//ball_velocity_y = -ball_velocity_y;
	//next_y = display_height - ball_width / 2;
    }


    if (ball_velocity_y > 0) {
        int i;
        for (i=counter-4; i<counter; ++i){
            //fprintf(stderr, "Ballpos: x=%g y=%g, Next_pos: x=%g, y=%g  --  ",ball_x, ball_y, next_x+30, next_y+30);
            //fprintf(stderr, "Stopps_pos: i=%d  x=%g y=%g, platform_pos: y=%g \n",i,stopps[i].stopper_x, stopps[i].stopper_y, platform_y[i]);
            
            if (stopps[i].stopper_y < 0+45){
                stopps[i].drew = 0;
                //fprintf(stderr, "This is the top stopper_y=%g \n",stopps[i].stopper_y);
            }

            if (((ball_y + ball_width) < stopps[i].stopper_y) && ((next_y + ball_width) >= platform_y[i])) {
                // Find what range of ball_y constitutes a bounce 
                range_top = stopps[i].stopper_x - ball_width;
                range_bottom = stopps[i].stopper_x + ball_width;

                if (ball_x > range_top && ball_x < range_bottom) {
                    
                    ball_velocity_y = 0.0f;
                    next_y = platform_y[i] - 30.08;
                    ground = 1;
                    jump = 0;
                }
            }
        }
    }
    // Bounce on right paddle? 
    if (ball_x + ball_width / 2 < paddle1_x
	&& next_x + ball_width / 2 >= paddle1_x) {
    }

    ball_x = next_x;
    ball_y = next_y;

    for (i=counter-4; i<counter; ++i){
        //stopps[i].stopper_x = platform_x;
        stopps[i].stopper_y = platform_y[i];
    }
    //fprintf(stderr, "Jump = %d, ground = %d \n",jump, ground);
    if (ball_y < 0 || ball_y > display_height) {
        //ball_init();
        fprintf(stderr, "Reset the game, this is the end \n ");
    }
}

/**
 * This function draws the player's view to the backbuffer.
 *
 * It is called as often as possible.
 */
void draw_frame()
{
    int i, j, r, g, b;
    int color;
    r = 0x00;
    g = 0x00;
    b = 0x00;
    color = makecol(r, g, b);
    set_alpha_blender();
/*
    for (i = 0; i < display_width; ++i) {
	for (j = 0; j < display_height; ++j) {
	    _putpixel32(bg_color, i, j, color);
	}
	if (!(i % 8)) {
	    --g;
	}
	color = makecol(r, g, b);
    }
*/

    // Draw Background color
    draw_sprite(backbuffer, bg_color, 0, 0);

    for (i = 0; i < display_width; ++i) {
	for (j = 0; j < display_height; ++j) {
	    if ((i % 20 == 0) && (j % 19 == 0)) {
		draw_sprite(backbuffer, brick, 0, j);
		draw_sprite(backbuffer, brick, (display_width - brick_x),j);
	    }
	}
    }
    // Stoppers
    //draw_trans_sprite(backbuffer, stoppers, (stopper_x - ball_width / 2), (stopper_y - ball_width / 2));
    
    for (i=counter-4; i<counter; ++i){
        fprintf(stderr, "Changed to new platform counter=%d \n",counter);
        if (stopps[i].drew != 0){
            //fprintf(stderr,"i=%d counter=%d \n",i, counter);
            draw_trans_sprite(backbuffer, stoppers, (stopps[i].stopper_x - ball_width /2 ), (stopps[i].stopper_y - ball_width /2) );
            //fprintf(stderr, "Stopps_pos: i=%d  x=%g y=%g \n",i,stopps[i].stopper_x, stopps[i].stopper_y);
        }else{
            ++counter;
            stopps[counter+1].drew = 1;
            //stopps[counter+1].stopper_x = 0.0f:
            //fprintf(stderr, "Changed to new platform counter=%d \n",counter);
        }
    }

    // The Ball
    draw_trans_sprite(backbuffer, red_ball, ball_x - ball_width / 2,
		      ball_y - ball_width / 2);

}

BITMAP *load_tga_or_die(const char *path)
{
    BITMAP *result;
    char tmppath[1024];

    result = load_tga(path, 0);

    if (!result) {

	strcpy(tmppath, DATADIR);
	strcat(tmppath, "/");
	strcat(tmppath, path);

	result = load_tga(tmppath, 0);
    }

    if (!result) {
	fprintf(stderr, "Failed to load image '%s'\n", path);

	exit(EXIT_FAILURE);
    }

    return result;
}

SAMPLE *load_wav_or_die(const char *path)
{
    SAMPLE *result;
    char tmppath[1024];

    if (!has_sound)
	return 0;

    result = load_wav(path);

    if (!result) {
	strcpy(tmppath, DATADIR);
	strcat(tmppath, "/");
	strcat(tmppath, path);

	result = load_wav(tmppath);
    }

    if (!result) {
	fprintf(stderr, "Failed to load sample '%s'\n", path);

	exit(EXIT_FAILURE);
    }

    return result;
}

void play_sound(SAMPLE * sound)
{
    if (sound && has_sound)
	play_sample(sound, 255, 0, 1000, 0);
}
