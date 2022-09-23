/*
 * display_DOGS_102.h
 *
 *  Created on: Jul 26, 2022
 *      Author: carrolls
 */

#ifndef INC_DISPLAY_DOGS_102_H_
#define INC_DISPLAY_DOGS_102_H_

#define CHECKS_WIDE (((uint16_t)8))
#define DISPLAY_WIDTH (((uint16_t) 102))

typedef struct {uint8_t nCD:1; uint8_t byte;} DOGS_packet;

void display_init(void);
void display_blank(void);
void display_checkerboard(void);
void display_snake_board(int8_t sb[CHECKS_WIDE][CHECKS_WIDE]);
void display_white_square(uint8_t l_to_r, uint8_t t_to_b);
void display_dark_square(uint8_t l_to_r, uint8_t t_to_b);


#endif /* INC_DISPLAY_DOGS_102_H_ */
