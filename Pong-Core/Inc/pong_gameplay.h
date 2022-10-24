/*
 * pong_gameplay.h
 *
 *  Created on: Jul 28, 2022
 *      Author: carrolls
 *      Edited: seand
 */

#ifndef PONG_GAMEPLAY_H_
#define PONG_GAMEPLAY_H_

#include <pong_enums.h>
#include <stdint.h>
#include <stdbool.h>
#include "smc_queue.h"
#include <pong_main.h> // Get checks and display width

#define PADDLE_WIDTH 1
#define PADDLE_HEIGHT 3

typedef struct {int16_t x; int16_t y;} XY_PT;

typedef struct{
	int8_t paddle_object;
    XY_PT loc;
} pong_paddle;

typedef struct{
	int8_t ball_object;
    XY_PT loc;
    enum pong_ball_dirs dir;
} pong_sphere;

typedef struct {
	int8_t board[CHECKS_WIDE][CHECKS_WIDE];
	pong_paddle paddle_L;
	pong_paddle paddle_R;
	pong_sphere ball;
} pong_board;

enum pong_ball_dirs pong_opposite_direction(pong_sphere ps);
void pong_game_init(pong_board* pb);
void paddle_shuffle(pong_paddle* pd, Smc_queue* q);
void paddle_R_shuffle(pong_board* pb, Smc_queue* q);
void pong_periodic_play(pong_board* pb);
bool pong_sphere_plot(pong_board *pb);
bool paddle_plot(pong_board *pb);

#endif /* PONG_GAMEPLAY_H_ */

