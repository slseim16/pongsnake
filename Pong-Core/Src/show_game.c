/*
 * show_snake.c
 *
 *  Created on: Sep 10, 2022
 *      Author: carrolls
 */

#include <pong_enums.h>
#include <pong_gameplay.h>
#include "display_DOGS_102.h"

void incremental_show_game(pong_board* pb, bool board_updated){		// Potentially useful for pong
	static int16_t x = 0;
	static int16_t y = 0;
//	static int8_t b[CHECKS_WIDE][CHECKS_WIDE] = {0};

	if (board_updated){
//		// clear canvas
//		for (int r = 0; r < CHECKS_WIDE; r++){
//			for (int c = 0; c < CHECKS_WIDE; c++){
//				b[r][c] = 0;
//			}
//		}

		//Clear the board
		for(int i=0; i<CHECKS_WIDE; i++){
			for(int j=0; j<CHECKS_WIDE; j++){
				pb->board[i][j] = 0;
			}
		}

		// paint canvas
		pong_sphere_plot(pb);
		paddle_plot(pb);
		// restart at top-left
		x = 0;
		y = 0;
	}
	if (pb->board[x][y] == 0){
		display_white_square(x,y);
	}
	else {
		display_dark_square(x,y);
	}

	// Update the statics so that the next plot is a new cell.
	x++;
	if (x >= CHECKS_WIDE){
		x = 0;
		y++;
		if (y >= CHECKS_WIDE){
			y = 0;
		}
	}
}
