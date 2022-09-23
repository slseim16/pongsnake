// MIT License - Sean Carroll - (c) 2022
// carrolls@trine.edu 2022.7.28 - merged into project
// carrolls@trine.edu 2022.07.04
// Game "snake - Eight-by-eight board
// Board encoding: -1 = fruit. 0 = empty. 1 = head;  2 = neck, ...
// so the snake's body comprises consecutive integers.

//// STATUS - Works.

//#define UNIT_TEST_SNAKE
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "snake_gameplay.h"
#include "snake_repair.h"
#include "quadknob.h"
#include "snake_enums.h"
#include "display_DOGS_102.h"

#define ERROR_DISPLAY_BAD_HEADING {{1,0},{1,1},{1,2}, {1,4},{1,5},{1,6}}
#define ERROR_DISPLAY_BLOCK_COUNT 6

// snake_opposite_direction(dir) is a utility N<->S, E<->W. It is used
// to convert head-to-tail guidance into tail-to-head direction-of-
// movement. The snake is stored h-to-t (because a player will be
// focused on the head). But when the snake moves, each vertebra
// will move toward the head - the reverse of directions here FROM
// the head
enum snake_compass_dir snake_opposite_direction(enum snake_compass_dir d){
	switch(d){
	case SNAKE_COMPASS_N: return SNAKE_COMPASS_S; break;
	case SNAKE_COMPASS_S: return SNAKE_COMPASS_N; break;
	case SNAKE_COMPASS_W: return SNAKE_COMPASS_E; break;
	case SNAKE_COMPASS_E: return SNAKE_COMPASS_W; break;
	default: return d;
	}
}


// snake_plot (snake_game, 2-d matrix of snake_compass dirs)
// Converts the snake from a list of vertebrae into a
// matrix of directions. 0 = empty, F = fruit, and the
// compass directions = a snake segment's direction of travel.
const XY_PT error_bar[ERROR_DISPLAY_BLOCK_COUNT] = ERROR_DISPLAY_BAD_HEADING;
bool snake_plot(const snake_game *s, int8_t b[CHECKS_WIDE][CHECKS_WIDE]){
	bool ok = true;
	int8_t x = s->head.x;
	int8_t y = s->head.y;
	b[x][y] = 1; // snake starts here: post a 1!

	// Plot each vertebra.
	for (int n = 0; n < (s->length - 1); n++){
		switch(s->vertebra[n]){
			case SNAKE_COMPASS_N: y--; break;
			case SNAKE_COMPASS_S: y++; break;
			case SNAKE_COMPASS_E: x++; break;
			case SNAKE_COMPASS_W: x--; break;
			default:
				for (int bc = 0; bc < ERROR_DISPLAY_BLOCK_COUNT; bc++){
				    display_dark_square(error_bar[bc].x, error_bar[bc].y);
				}
		}
		x = (x >= 0)?(x % CHECKS_WIDE):(CHECKS_WIDE-1);
		y = (y >= 0)?(y % CHECKS_WIDE):(CHECKS_WIDE-1);
		if (b[x][y] > 0) ok = false; // overlapping vertebrae NOT ok
		b[x][y] = n+2;
	}
	return ok;
}

bool fruit_plot(const snake_game *s, int8_t b[CHECKS_WIDE][CHECKS_WIDE]){
	// If the fruit already plotted, or the ground is clear = ok!
	bool ok = true;
	if (b[s->fruit.x][s->fruit.y] == -1) ok = true;
	else if (b[s->fruit.x][s->fruit.y] == 0){
		ok = true;
		b[s->fruit.x][s->fruit.y] = -1;
	}
	else{
		ok = false;
	}
	return ok;
}


// snake_board_init (obvious - sets head, tail, vertebra directions & length, first fruit)
void snake_game_init(snake_game* s){
	const XY_PT initial_head = {3,3};
	const XY_PT initial_fruit = {3,6};
	const int8_t initial_length = 3;
	// JPL DEVIATION - ACCEPT WARNING: "Avoid constant literals"
	// Excuse -- The snake's initial position is (1) initialization,
	// not operation; and (2) it is arbitrary, not critical -- so defining
	// named constants for this is silly. Just be sure that each
	// coordinate is within {0 ... CHECKS_WIDE-1}.
	s->head.x = initial_head.x;
	s->head.y = initial_head.y;
	s->vertebra[0] = SNAKE_COMPASS_N; // This is the head. The neck is NORTH
	s->vertebra[1] = SNAKE_COMPASS_W; // The body @ 1. The tail is WEST.
	s->length = initial_length;
	s->fruit.x = initial_fruit.x;
	s->fruit.y = initial_fruit.y;
	s->heading = SNAKE_COMPASS_S; // Start off toward the bottom.
}


void pacify_compiler(){

}

// snake_heading_update(game-pointer, queue-pointer) will adjust the
// (.heading) field of a snake in response to a message from the user
// as conveyed on the queue.
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


void snake_place_fruit(snake_game *s, const int8_t board[CHECKS_WIDE][CHECKS_WIDE]){
	// MUST NOT CALL snake_board_cleanup to avoid bad recursion.
	// Tries to place fruit randomly; if unlucky, then switches to a
	// put-it-in-the-first-empty-cell.
	const int16_t patience = 100;
	bool complete = false;
	// JPL says all loops must terminate - so we will only try 100 times
	// (called our "patience") for a fun random result. Otherwise, we will
	// place fruit at the first available cell l-to-r,  top-to-bottom.
	for (int n = 0; n < patience; n++){
		uint8_t yy = rand() % CHECKS_WIDE;
		uint8_t xx = rand() % CHECKS_WIDE;
		if (board[xx][yy] == 0){
			complete = true;
			s->fruit.x = xx;
			s->fruit.y = yy;
			break; //-- done the loop!
		} // end if board cell is open
	}// end for loop trying 100 random placements

	// If 100 random guesses all hit the snake's body, then
	// begin an exhaustive search from the top left. We will
	// find an open cell if there is one!
	if (!complete){
		for (int r = 0; r < CHECKS_WIDE; r++){
			for (int c = 0; c < CHECKS_WIDE; c++){
				if (board[r][c] == 0){
					complete = true;
					s->fruit.x = r;
					s->fruit.y = c;
					break; //-- done the loop!
				}// end if
			} // end for-c
		} // end for-r
	} // end if back-up plan
	display_dark_square(s->fruit.x,s->fruit.y);

	return;
}

XY_PT find_next_head(snake_game* s){
	XY_PT square = s->head;
	switch(s->heading){
	    // The heading should always be a real direction
		case SNAKE_COMPASS_0:
		case SNAKE_COMPASS_FRUIT_HERE:
			snake_game_init(s);
			break;
		// Quick calculation of the next cell:
		case SNAKE_COMPASS_N:
			square.y--;
			break;
		case SNAKE_COMPASS_S:
			square.y++;
			break;
		case SNAKE_COMPASS_E:
			square.x++;
			break;
		case SNAKE_COMPASS_W:
			square.x--;
			break;
	}
	// Wrap like a torus: -1->7, 0->0, 1->1, 7->7, 8->0
	square.x = (square.x >= 0)?(square.x % CHECKS_WIDE):(CHECKS_WIDE-1);
	square.y = (square.y >= 0)?(square.y % CHECKS_WIDE):(CHECKS_WIDE-1);
	return square;
}


void snake_periodic_play(snake_game* s){
	// Get a fresh plot of the board to check for legal & fruit moves:
	static int8_t board[CHECKS_WIDE][CHECKS_WIDE];
	// Always clear the board and redraw it.
	for (int x = 0; x < CHECKS_WIDE; x++){
		for (int y = 0; y < CHECKS_WIDE; y++){
			board[x][y] = 0;
		}
	}
	bool ok;
	ok = snake_plot(s, board) && fruit_plot(s, board); // Will happen l-to-r.
	if (!ok) {
		display_checkerboard();
		for (volatile int32_t n = 0 ; n< BIG_DELAY_COUNT; n++);
		snake_game_init(s);
		snake_plot(s, board);
	}

	XY_PT next_head = find_next_head(s);

	// Check for snake self-bite
	if (board[next_head.x][next_head.y] >= 1){
		// CRASH!
		while(1);
	}

	// Is the heading a normal move into an empty cell?
	else if (board[next_head.x][next_head.y] == 0){
		s->head.x = next_head.x;
		s->head.y = next_head.y;
		// Start at the last element in the spine.
		// Vertebra[n] moves forward into the cell
		// previously occuppied by Vertebra[n-1], and
		// it inherits not only the cell, but also the
		// heading of Vertebra[n-1] - so copy the heading
		// from V[n-1] to V[n], and then let the head, AKA
		// Vertebra[0], take the user-controlled Heading as
		// its direction.
		// There are length-1 Vertebrae, but only length-2
		// connections between them.
		for (int n = (s->length - 2); n>0; n--){
			s->vertebra[n] = s->vertebra[n-1];
		}
		s->vertebra[0] = snake_opposite_direction(s->heading);
	}

	// Is this a move into fruit?
	else if (board[next_head.x][next_head.y] == -1)
	{
		s->length++;
		snake_place_fruit(s, (const int8_t(*)[CHECKS_WIDE]) board);
		s->head.x = next_head.x;
		s->head.y = next_head.y;
		// Slither all vertebrae fwd with a for-loop and add a new
		// head - and keep the old tail.
		for (int n = (s->length - 2); n>0; n--){
			s->vertebra[n] = s->vertebra[n-1];
		}
		s->vertebra[0] = snake_opposite_direction(s->heading);
	}
}

