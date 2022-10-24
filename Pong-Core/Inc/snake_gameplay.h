/*
 * snake_gameplay.h
 *
 *  Created on: Jul 28, 2022
 *      Author: carrolls
 */

#ifndef SNAKE_GAMEPLAY_H_
#define SNAKE_GAMEPLAY_H_

#include <stdint.h>
#include <stdbool.h>
#include "Quadknob.h"
#include "snake_enums.h"
#include "smc_queue.h"
#include "snake_main.h" // Get checks and display width

typedef struct {int16_t x; int16_t y;} XY_PT;

typedef struct {
	enum snake_compass_dir vertebra[CHECKS_WIDE * CHECKS_WIDE];
	XY_PT head;
	XY_PT fruit;
	uint8_t length;
	enum snake_compass_dir heading;
} snake_game;

enum snake_compass_dir snake_opposite_direction(enum snake_compass_dir d);
void snake_game_init(snake_game* s);
void snake_game_cleanup(snake_game* s);
void snake_heading_update(snake_game* snake, Smc_queue* q);
void snake_periodic_play(snake_game *snake);
void snake_place_fruit(snake_game * s, const int8_t b[CHECKS_WIDE][CHECKS_WIDE]);
bool snake_plot(const snake_game *s, int8_t b[CHECKS_WIDE][CHECKS_WIDE]);
bool fruit_plot(const snake_game *s, int8_t b[CHECKS_WIDE][CHECKS_WIDE]);

#endif /* SNAKE_GAMEPLAY_H_ */

