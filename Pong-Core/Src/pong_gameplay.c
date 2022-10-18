// MIT License - Sean Carroll - (c) 2022
// carrolls@trine.edu 2022.7.28 - merged into project
// carrolls@trine.edu 2022.07.04
// Edited: seand
// Game "pong" - Eight-by-eight board
// Board encoding: -1 = sphere(ball). 0 = empty. 1 = Left Paddle;  2 = Right Paddle

//// STATUS - ?????.

//#define UNIT_TEST_SNAKE
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
enum pong_ball_dirs pong_opposite_direction(enum pong_ball_dirs d){
// This encourages the ball to spin in a clockwise direction on the board
	switch(d){
	case N: return S; break;
	case NE: return NW; break;
	case E: return W; break;
	case SE: return NE; break;
	case S: return N; break;
	case SW: return SE; break;
	case W: return E; break;
	case NW: return SW; break;
	default: return d;
	}
// This knocks the ball back in the same direction it came
//	switch(d){
//	case N: return S; break;
//	case NE: return SW; break;
//	case E: return W; break;
//	case SE: return NW; break;
//	case S: return N; break;
//	case SW: return NE; break;
//	case W: return E; break;
//	case NW: return SE; break;
//	default: return d;
//	}
}


// snake_plot (snake_game, 2-d matrix of snake_compass dirs)
// Converts the snake from a list of vertebrae into a
// matrix of directions. 0 = empty, F = fruit, and the
// compass directions = a snake segment's direction of travel.
const XY_PT error_bar[ERROR_DISPLAY_BLOCK_COUNT] = ERROR_DISPLAY_BAD_HEADING;		//Used for error codes
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
	default:		//Error handling, pacify the compiler
		//pacify_compiler();
		break;
	}

	pb->ball.loc.x = x;
	pb->ball.loc.y = y;
	pb->board[x][y] = pb->ball.ball_object;


//	// Plot each vertebra.
//	for (int n = 0; n < (s->length - 1); n++){
//		switch(s->vertebra[n]){
//			case SNAKE_COMPASS_N: y--; break;
//			case SNAKE_COMPASS_S: y++; break;
//			case SNAKE_COMPASS_E: x++; break;
//			case SNAKE_COMPASS_W: x--; break;
//			default:
//				for (int bc = 0; bc < ERROR_DISPLAY_BLOCK_COUNT; bc++){
//				    display_dark_square(error_bar[bc].x, error_bar[bc].y);
//				}
//		}
//		x = (x >= 0)?(x % CHECKS_WIDE):(CHECKS_WIDE-1);
//		y = (y >= 0)?(y % CHECKS_WIDE):(CHECKS_WIDE-1);
//		if (b[x][y] > 0) ok = false; // overlapping vertebrae NOT ok
//		b[x][y] = n+2;
//	}
	return ok;
}

bool paddle_plot(pong_board *pb){
//	// If the fruit already plotted, or the ground is clear = ok!
	bool ok = true;
//	if (b[s->fruit.x][s->fruit.y] == -1) ok = true;
//	else if (b[s->fruit.x][s->fruit.y] == 0){
//		ok = true;
//		b[s->fruit.x][s->fruit.y] = -1;
//	}
//	else{
//		ok = false;
//	}

	//Get the loc for L&R paddles
	int8_t lx = pb->paddle_L.loc.x;
	int8_t ly = pb->paddle_L.loc.y;

	int8_t rx = pb->paddle_R.loc.x;
	int8_t ry = pb->paddle_R.loc.y;

	//Guard clause for paddles NOTE MAGIC NUMBERS
	if(pb->paddle_L.loc.y <0 || pb->paddle_L.loc.y > 5){
//		ok = false;
//		return ok;
		return ok;
	}

	if(pb->paddle_R.loc.y <0 || pb->paddle_R.loc.y > 5){
//		ok = false;
//		return ok;
		return ok;
	}

	//Plots the paddles
	for(int i=0; i<PADDLE_WIDTH; i++){
		for(int j=0; j<PADDLE_HEIGHT; j++){
			pb->board[lx + i][ly + j] = pb->paddle_L.paddle_object;
		}
	}

	for(int i=0; i<PADDLE_WIDTH; i++){
		for(int j=0; j<PADDLE_HEIGHT; j++){
			pb->board[rx + i][ry + j] = pb->paddle_R.paddle_object;
		}
	}

	pb->paddle_L.loc.x = lx;
	pb->paddle_L.loc.y = ly;
	pb->paddle_R.loc.x = rx;
	pb->paddle_R.loc.y = ry;

	return ok;
}


void paddle_L_shuffle(pong_board* pb, Smc_queue* q){
	//State machine for determining how to modify the loc of paddle_L then plotting it

	//Checks if there is data on the queue
	Q_data msg;
	bool data_available;
	data_available = q->get(q, &msg);
    if (!data_available) return;

	//Guard clause for paddles NOTE MAGIC NUMBERS
	if(pb->paddle_L.loc.y <0 || pb->paddle_L.loc.y > 5){
		return;
	}

	switch(msg.movement){
	case UP:
		pb->paddle_L.loc.y--;
		break;
	case DOWN:
		pb->paddle_L.loc.y++;
		break;
	default:
		break;
	}
//	paddle_plot(pb);
}

void paddle_R_shuffle(pong_board* pb, Smc_queue* q){
	//State machine for determining how to modify the loc of paddle_L then plotting it

	//Checks if there is data on the queue
	Q_data msg;
	bool data_available;
	data_available = q->get(q, &msg);
    if (!data_available) return;

	//Guard clause for paddles NOTE MAGIC NUMBERS
	if(pb->paddle_R.loc.x <0 || pb->paddle_R.loc.x > 5){
		return;
	}

	switch(msg.movement){
	case UP:
		pb->paddle_R.loc.x++;
		break;
	case DOWN:
		pb->paddle_R.loc.x--;
		break;
	default:
		break;
	}
//	paddle_plot(pb);
}

// snake_board_init (obvious - sets head, tail, vertebra directions & length, first fruit)
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
	// TODO code logic for sphere hitting columns 0 and 7 (x=0 and x=7)

	const XY_PT paddle_L = {0,2};
	const XY_PT paddle_R = {7,3};
	const XY_PT sphere = {3,3};

//	const XY_PT initial_head = {3,3};
//	const XY_PT initial_fruit = {3,6};
//	const int8_t initial_length = 3;
	// JPL DEVIATION - ACCEPT WARNING: "Avoid constant literals"
	// Excuse -- The snake's initial position is (1) initialization,
	// not operation; and (2) it is arbitrary, not critical -- so defining
	// named constants for this is silly. Just be sure that each
	// coordinate is within {0 ... CHECKS_WIDE-1}.
//	s->head.x = initial_head.x;
//	s->head.y = initial_head.y;
//	s->vertebra[0] = SNAKE_COMPASS_N; // This is the head. The neck is NORTH
//	s->vertebra[1] = SNAKE_COMPASS_W; // The body @ 1. The tail is WEST.
//	s->length = initial_length;
//	s->fruit.x = initial_fruit.x;
//	s->fruit.y = initial_fruit.y;
//	s->heading = SNAKE_COMPASS_S; // Start off toward the bottom.

	//Initialize the pong board empty squares
	for(int i=0; i<CHECKS_WIDE; i++){
		for(int j=0; j<CHECKS_WIDE; j++){
			pb->board[i][j]=0;
		}
	}

	//Initializes the Paddles
//	for(int i=0; i<PADDLE_WIDTH; i++){
//		for(int j=0; j<PADDLE_HEIGHT; j++){
//			pb->paddle_L.paddle_object[i][j] = 1;
//		}
//	}
//	for(int i=0; i<PADDLE_WIDTH; i++){
//		for(int j=0; j<PADDLE_HEIGHT; j++){
//			pb->paddle_R.paddle_object[i][j] = 2;
//		}
//	}
	pb->paddle_L.loc = paddle_L;
	pb->paddle_L.paddle_object = 1;
	pb->paddle_R.loc = paddle_R;
	pb->paddle_R.paddle_object = 2;


	//Initializes the sphere
	pb->ball.ball_object = -1;
	pb->ball.loc = sphere;
	pb->ball.dir = SW;

	paddle_plot(pb);
	pong_sphere_plot(pb);
}

/*
void pacify_compiler(){

}
*/
// snake_heading_update(game-pointer, queue-pointer) will adjust the
// (.heading) field of a snake in response to a message from the user
// as conveyed on the queue.
/*
void snake_heading_update(snake_game* s, Smc_queue* q){
	Q_data msg;
	bool data_available;
	data_available = q->get(q, &msg);
    if (!data_available) return;
    else{
    	switch(s->heading){
    	case SNAKE_COMPASS_N:
    		s->heading = (msg.twist == QUADKNOB_CW)?
    					 SNAKE_COMPASS_E:SNAKE_COMPASS_W;
    		break;
    	case SNAKE_COMPASS_E:
    		s->heading = (msg.twist == QUADKNOB_CW)?
    					 SNAKE_COMPASS_S:SNAKE_COMPASS_N;
    		break;
    	case SNAKE_COMPASS_S:
    		s->heading = (msg.twist == QUADKNOB_CW)?
    					 SNAKE_COMPASS_W:SNAKE_COMPASS_E;
    		break;
    	case SNAKE_COMPASS_W:
    		s->heading = (msg.twist == QUADKNOB_CW)?
    					 SNAKE_COMPASS_N:SNAKE_COMPASS_S;
    		break;
    	default: //s->heading remains unchanged. No good way to say this in C.
    		pacify_compiler();
    	}
    }
}
*/

//void snake_place_fruit(snake_game *s, const int8_t board[CHECKS_WIDE][CHECKS_WIDE]){
//	// MUST NOT CALL snake_board_cleanup to avoid bad recursion.
//	// Tries to place fruit randomly; if unlucky, then switches to a
//	// put-it-in-the-first-empty-cell.
//	const int16_t patience = 100;
//	bool complete = false;
//	// JPL says all loops must terminate - so we will only try 100 times
//	// (called our "patience") for a fun random result. Otherwise, we will
//	// place fruit at the first available cell l-to-r,  top-to-bottom.
//	for (int n = 0; n < patience; n++){
//		uint8_t yy = rand() % CHECKS_WIDE;
//		uint8_t xx = rand() % CHECKS_WIDE;
//		if (board[xx][yy] == 0){
//			complete = true;
//			s->fruit.x = xx;
//			s->fruit.y = yy;
//			break; //-- done the loop!
//		} // end if board cell is open
//	}// end for loop trying 100 random placements
//
//	// If 100 random guesses all hit the snake's body, then
//	// begin an exhaustive search from the top left. We will
//	// find an open cell if there is one!
//	if (!complete){
//		for (int r = 0; r < CHECKS_WIDE; r++){
//			for (int c = 0; c < CHECKS_WIDE; c++){
//				if (board[r][c] == 0){
//					complete = true;
//					s->fruit.x = r;
//					s->fruit.y = c;
//					break; //-- done the loop!
//				}// end if
//			} // end for-c
//		} // end for-r
//	} // end if back-up plan
//	display_dark_square(s->fruit.x,s->fruit.y);
//
//	return;
//}

//XY_PT find_next_head(snake_game* s){
//	XY_PT square = s->head;
//	switch(s->heading){
//	    // The heading should always be a real direction
//		case SNAKE_COMPASS_0:
//		case SNAKE_COMPASS_FRUIT_HERE:
//			pong_game_init(s);
//			break;
//		// Quick calculation of the next cell:
//		case SNAKE_COMPASS_N:
//			square.y--;
//			break;
//		case SNAKE_COMPASS_S:
//			square.y++;
//			break;
//		case SNAKE_COMPASS_E:
//			square.x++;
//			break;
//		case SNAKE_COMPASS_W:
//			square.x--;
//			break;
//	}
//	// Wrap like a torus: -1->7, 0->0, 1->1, 7->7, 8->0
//	square.x = (square.x >= 0)?(square.x % CHECKS_WIDE):(CHECKS_WIDE-1);
//	square.y = (square.y >= 0)?(square.y % CHECKS_WIDE):(CHECKS_WIDE-1);
//	return square;
//}

void pong_periodic_play(pong_board* pb){
//	// Get a fresh plot of the board to check for legal & fruit moves:
//	static int8_t board[CHECKS_WIDE][CHECKS_WIDE];
//	// Always clear the board and redraw it.
	for (int x = 0; x < CHECKS_WIDE; x++){
		for (int y = 0; y < CHECKS_WIDE; y++){
			pb->board[x][y] = 0;
		}
	}
	bool ok;
	ok = pong_sphere_plot(pb) && paddle_plot(pb); // Will happen l-to-r. NOTE as-is it will always be true
	if (!ok) {
		display_checkerboard();
		for (volatile int32_t n = 0 ; n< BIG_DELAY_COUNT; n++);
		pong_game_init(pb);
		pong_sphere_plot(pb);
	}


	//Checking sphere collision (is the sphere moving into a paddle, vertical wall, or ceiling/floor?)
	//NOTE use of MAGIC NUMBERS 0 and 7-- The far coordinates
	if((pb->ball.loc.x == (pb->paddle_L.loc.x + 1)) && (pb->ball.loc.y >= pb->paddle_L.loc.y && pb->ball.loc.y <= (pb->paddle_L.loc.y + PADDLE_HEIGHT - 1))){
		pb->ball.dir = pong_opposite_direction(pb->ball.dir);
	}

	if((pb->ball.loc.x == (pb->paddle_R.loc.x - 1)) && (pb->ball.loc.y >= pb->paddle_R.loc.y && pb->ball.loc.y <= (pb->paddle_R.loc.y + PADDLE_HEIGHT - 1))){
		pb->ball.dir = pong_opposite_direction(pb->ball.dir);
	}

	if(pb->ball.loc.y == 0){
		pb->ball.dir = pong_opposite_direction(pb->ball.dir);
//		display_checkerboard();
	}
	if(pb->ball.loc.y == 7){
		pb->ball.dir = pong_opposite_direction(pb->ball.dir);
//		display_checkerboard();
	}

	//NOTE THESE CONDITIONS END THE GAME, GAME MUST BE RESET
	if(pb->ball.loc.x == 0){
//		pb->ball.dir = pong_opposite_direction(pb->ball.dir);
		display_checkerboard();
		while(1);
	}
	if(pb->ball.loc.x == 7){
//		pb->ball.dir = pong_opposite_direction(pb->ball.dir);
		display_checkerboard();
		while(1);
	}

//	XY_PT next_head = find_next_head(s);

//	// Check for snake self-bite
//	if (board[next_head.x][next_head.y] >= 1){
//		// CRASH!
//		while(1);
//	}

//	// Is the heading a normal move into an empty cell?
//	else if (board[next_head.x][next_head.y] == 0){
//		s->head.x = next_head.x;
//		s->head.y = next_head.y;
//		// Start at the last element in the spine.
//		// Vertebra[n] moves forward into the cell
//		// previously occuppied by Vertebra[n-1], and
//		// it inherits not only the cell, but also the
//		// heading of Vertebra[n-1] - so copy the heading
//		// from V[n-1] to V[n], and then let the head, AKA
//		// Vertebra[0], take the user-controlled Heading as
//		// its direction.
//		// There are length-1 Vertebrae, but only length-2
//		// connections between them.
//		for (int n = (s->length - 2); n>0; n--){
//			s->vertebra[n] = s->vertebra[n-1];
//		}
//		s->vertebra[0] = pong_opposite_direction(s->heading);
//	}

//	// Is this a move into fruit?
//	else if (board[next_head.x][next_head.y] == -1)
//	{
//		s->length++;
//		snake_place_fruit(s, (const int8_t(*)[CHECKS_WIDE]) board);
//		s->head.x = next_head.x;
//		s->head.y = next_head.y;
//		// Slither all vertebrae fwd with a for-loop and add a new
//		// head - and keep the old tail.
//		for (int n = (s->length - 2); n>0; n--){
//			s->vertebra[n] = s->vertebra[n-1];
//		}
//		s->vertebra[0] = pong_opposite_direction(s->heading);
//	}
}

