/*
 * snake_repair.c
 *
 *  Created on: Sep 10, 2022
 *      Author: carrolls
 */

#include <pong_gameplay.h>
#include "display_DOGS_102.h"

/*
void snake_game_cleanup(snake_game* s){
	display_init();

	// Illegal heading? Then go straight ahead.
	switch(s->heading){
		case SNAKE_COMPASS_N:
		case SNAKE_COMPASS_S:
		case SNAKE_COMPASS_E:
		case SNAKE_COMPASS_W: break;
		default:
			s->heading = pong_opposite_direction(s->vertebra[0]);
	}

	// Check that the snake doesn't cross itself:
	int8_t board[CHECKS_WIDE][CHECKS_WIDE] = {{0}};

	int8_t x = s->head.x;
	int8_t y = s->head.y;
	if ((x > CHECKS_WIDE-1)||(y> CHECKS_WIDE -1)){
		// Impossible head location: re-spawn game.
		pong_game_init(s);
		x = s->head.x;
		y = s->head.y;
	}
	// Plot & display the head
	board[x][y] = 1;
	display_dark_square(x,y);

	// Plot & show the body - but no the poop after the tail.
	// We are looking for a possible self-crossing of a snake in 2-D.
	for (int n = 0; n < (s->length - 1); n++){
		switch(s->vertebra[n]){
			case SNAKE_COMPASS_N: y--; break;
			case SNAKE_COMPASS_S: y++; break;
			case SNAKE_COMPASS_E: x++; break;
			case SNAKE_COMPASS_W: x--; break;
			default: s->vertebra[n] = SNAKE_COMPASS_N; y--; break;
		}
		x = (x >= 0)?(x % CHECKS_WIDE):(CHECKS_WIDE-1);
		y = (y >= 0)?(y % CHECKS_WIDE):(CHECKS_WIDE-1);
		if (board[x][y] != 0){
			// Snake cannot cross itself; truncate the snake here.
			// Do not move the tail. It's OK to change s->length
			// because we are leaving this loop.
			s->length = n+1;
			break;
		}
		else {
			// Mark the board with a non-zero value
			board[x][y] = n+1;
		}
		display_dark_square(x,y);
	}

	// Ensure fruit placement
	if (board[s->fruit.x][s->fruit.y] != 0){
		snake_place_fruit(s, (const int8_t(*)[CHECKS_WIDE])board);
	}
	display_dark_square(s->fruit.x, s->fruit.y);
}
*/
