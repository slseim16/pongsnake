// MIT License - Sean Carroll - (c) 2022
// carrolls@trine.edu 2022.7.28 - merged into project
// carrolls@trine.edu 2022.07.04
// Edited: seand
// Game "pong" - Eight-by-eight board
// Board encoding: -1 = sphere(ball). 0 = empty. 1 = Left Paddle;  2 = Right Paddle

//// STATUS - works.

#include "main.h"
#include <pong_enums.h>
#include <pong_gameplay.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "snake_repair.h"
#include "display_DOGS_102.h"

#define ERROR_DISPLAY_BAD_HEADING {{1,0},{1,1},{1,2}, {1,4},{1,5},{1,6}}
#define ERROR_DISPLAY_BLOCK_COUNT 6

// pong_opposite_direction(d) is a utility based on snake_opposite_direction (loosely).
// It is used to change direction of pong ball when it collides with a paddle
// or the ceiling/floor.
enum pong_ball_dirs pong_opposite_direction(pong_sphere ps){
	enum pong_ball_dirs d = ps.dir;
	//Checks the ball's collision, first checks to see if it's in any of the four corners
	//	Then checks to see if it's hit the ceiling or floor,
	//  Then checks to see if it's in the column right next to either paddle.
	//This should handle all possible usuable inputs for pong, but if not move to the switch in the else statement.
	if(ps.loc.x == 1 && ps.loc.y == 0){return SE;}

	if(ps.loc.x == 6 && ps.loc.y == 0){return SW;}

	if(ps.loc.x == 1 && ps.loc.y == 7){return NE;}

	if(ps.loc.x == 6 && ps.loc.y == 7){return NW;}

	else if (ps.loc.y == 0){
		switch(d){
		case NE:
			return SE; break;
		case NW:
			return SW; break;
		default:
			return d;
		}
	}
	else if (ps.loc.y == 7){
		switch(d){
		case SE:
			return NE; break;
		case SW:
			return NW; break;
		default:
			return d;
		}
	}
	else if (ps.loc.x == 1){
		switch(d){
		case NW:
			return NE; break;
		case SW:
			return SE; break;
		default:
			return d;
		}
	}
	else if (ps.loc.x == 6){
		switch(d){
		case NE:
			return NW; break;
		case SE:
			return SW; break;
		default:
			return d;
		}
	}
	else{
		switch(d){
		case N: return S; break;
		case NE: return SW; break;
		case E: return W; break;
		case SE: return NW; break;
		case S: return N; break;
		case SW: return NE; break;
		case W: return E; break;
		case NW: return SE; break;
		default: return d;
		}
	}
}

bool pong_sphere_plot(pong_board *pb){
	bool ok = true;

	int8_t x = pb->ball.loc.x;
	int8_t y = pb->ball.loc.y;

	//Guard clause for illegal bounds
	if(x <0 || y<0 || x>7 || y>7){
		ok = false;
		return ok;
	}

	switch(pb->ball.dir){
	case N:
		y--;
		break;
	case NE:
		x++;
		y--;
		break;
	case E:
		x++;
		break;
	case SE:
		x++;
		y++;
		break;
	case S:
		y++;
		break;
	case SW:
		x--;
		y++;
		break;
	case W:
		x--;
		break;
	case NW:
		x--;
		y--;
		break;
	default:		//Realistically shouldn't reach here, so don't do anything
		break;
	}

	//Record new locations
	pb->ball.loc.x = x;
	pb->ball.loc.y = y;
	pb->board[x][y] = pb->ball.ball_object;
	return ok;
}

bool paddle_plot(pong_board *pb){
	bool ok = true;

	//Get the loc for L&R paddles
	int8_t lx = pb->paddle_L.loc.x;
	int8_t ly = pb->paddle_L.loc.y;

	int8_t rx = pb->paddle_R.loc.x;
	int8_t ry = pb->paddle_R.loc.y;

	//Plots the paddles
	for(uint8_t i=0; i<PADDLE_WIDTH; i++){
		for(uint8_t j=0; j<PADDLE_HEIGHT; j++){
			pb->board[lx + i][ly + j] = pb->paddle_L.paddle_object;
		}
	}

	for(uint8_t i=0; i<PADDLE_WIDTH; i++){
		for(uint8_t j=0; j<PADDLE_HEIGHT; j++){
			pb->board[rx + i][ry + j] = pb->paddle_R.paddle_object;
		}
	}

	//Record new locations
	pb->paddle_L.loc.x = lx;
	pb->paddle_L.loc.y = ly;
	pb->paddle_R.loc.x = rx;
	pb->paddle_R.loc.y = ry;

	return ok;
}


void paddle_shuffle(pong_paddle* pd, Smc_queue* q){
	//State machine for determining how to modify the loc of paddle_L then plotting it

	//Checks if there is data on the queue
	Q_data msg;
	bool data_available;
	data_available = q->get(q, &msg);
    if (!data_available) return;

    //Store the future y value, essential to keep the paddles from disappearing from this plane of existence
    int16_t y_future;

	switch(msg.movement){
	case UP:
		y_future = pd->loc.y + 1;
		if(y_future < 0 || y_future > 5){
			return;
		}
		pd->loc.y++;
		break;
	case DOWN:
		y_future = pd->loc.y - 1;
		if(y_future < 0 || y_future > 5){
			return;
		}
		pd->loc.y--;
		break;
	default:
		break;
	}
}

void pong_game_init(pong_board* pb){
	// BOARD STRUCTURE
	// The board is set up in an 8x8 matrix below
	//
	// 0 1 2 3 4 5 6 7
	// 0 0 0 0 0 0 0 0 0
	// 0 0 0 0 0 0 0 0 1
	// 1 0 0 0 0 0 0 0 2
	// 1 0 0 i 0 0 0 2 3
	// 1 0 0 0 0 0 0 2 4
	// 0 0 0 0 0 0 0 2 5
	// 0 0 0 0 0 0 0 0 6
	// 0 0 0 0 0 0 0 0 7
	//
	// NOTE! ARRAYS IN C DON'T BEHAVE LIKE THIS!!
	// To plot the top most 1, it must be placed in b[2][0]
	//
	// The 1's and 2's represent where the PADDLES should draw and the i(-1) where the sphere should draw
	// Each paddle object has a location enum of XY_PT as well as the sphere
	// With the paddles being a three block tall entity, it's location is determined by its topmost block
	// with y bounds of 0<= loc.y <= 5
	//
	// The sphere has y bounds of 0<= loc.x <= 7 && 0<= loc.y <=7 assuming it runs into no paddles.

	const XY_PT paddle_L = {0,4};
	const XY_PT paddle_R = {7,3};
	const XY_PT sphere = {3,2};

	//Initialize the pong board empty squares
	for(uint8_t i=0; i<CHECKS_WIDE; i++){
		for(uint8_t j=0; j<CHECKS_WIDE; j++){
			pb->board[i][j]=0;
		}
	}

	pb->paddle_L.loc = paddle_L;
	pb->paddle_L.paddle_object = 1;
	pb->paddle_R.loc = paddle_R;
	pb->paddle_R.paddle_object = 2;


	//Initializes the sphere
	pb->ball.ball_object = -1;
	pb->ball.loc = sphere;
	pb->ball.dir = SW;

	//Plot the paddles and the sphere
	paddle_plot(pb);
	pong_sphere_plot(pb);
}

void pong_periodic_play(pong_board* pb){
	//Clear the board
	for (uint8_t x = 0; x < CHECKS_WIDE; x++){
		for (uint8_t y = 0; y < CHECKS_WIDE; y++){
			pb->board[x][y] = 0;
		}
	}
	bool ok;
	ok = pong_sphere_plot(pb) && paddle_plot(pb); //Verifies the paddles and board are plotted
	if (!ok) {
		display_checkerboard();
		for (volatile int32_t n = 0 ; n< BIG_DELAY_COUNT; n++);
		pong_game_init(pb);
		pong_sphere_plot(pb);
	}

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	//Checking sphere collision (is the sphere moving into a paddle, vertical wall, or ceiling/floor?)
	//NOTE use of MAGIC NUMBERS 0 and 7-- The far coordinates
	if((pb->ball.loc.x == (pb->paddle_L.loc.x + 1)) && (pb->ball.loc.y >= pb->paddle_L.loc.y && pb->ball.loc.y <= (pb->paddle_L.loc.y + PADDLE_HEIGHT - 1))){
		pb->ball.dir = pong_opposite_direction(pb->ball);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	}

	if((pb->ball.loc.x == (pb->paddle_R.loc.x - 1)) && (pb->ball.loc.y >= pb->paddle_R.loc.y && pb->ball.loc.y <= (pb->paddle_R.loc.y + PADDLE_HEIGHT - 1))){
		pb->ball.dir = pong_opposite_direction(pb->ball);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	}

	else if(pb->ball.loc.y == 0){
		pb->ball.dir = pong_opposite_direction(pb->ball);
	}
	else if(pb->ball.loc.y == 7){
		pb->ball.dir = pong_opposite_direction(pb->ball);
	}

	//NOTE THESE CONDITIONS END THE GAME, GAME MUST BE RESET WITH HW
	else if(pb->ball.loc.x < 0){
		display_checkerboard();
		while(1);
	}
	else if(pb->ball.loc.x > 7){
		display_checkerboard();
		while(1);
	}
}
