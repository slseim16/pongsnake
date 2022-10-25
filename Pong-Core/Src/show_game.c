/*
 * show_snake.c
 *
 *  Created on: Sep 10, 2022
 *      Author: carrolls
 *      Edited: seand
 */

#include <pong_enums.h>
#include <pong_gameplay.h>
#include "display_DOGS_102.h"

void incremental_show_game(pong_board* pb, bool board_updated){
	static int16_t x = 0;
	static int16_t y = 0;

	if (board_updated){
		//Clear the board
		for(uint8_t i=0; i<CHECKS_WIDE; i++){
			for(uint8_t j=0; j<CHECKS_WIDE; j++){
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
