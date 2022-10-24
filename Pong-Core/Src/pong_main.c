// Pong game demo for STM Board Nucleo-G071RB
// Based off Snake game demo
// Sean, Spencer, and Bradyn
// carrolls@trine.edu
// Copyright 2022 Sean Carroll
// 2022.7.26
//
// This demo is based off the SNAKE demo, and as such borrows most of its drivers and structure
//


#include <stdint.h>
#include <stdlib.h> // rand
#include <stdbool.h>
#include <cmsis_gcc.h>
#include <pong_enums.h>
#include <pong_gameplay.h>
#include <pong_main.h>
#include <show_game.h>
#include "main.h"
#include "display_DOGS_102.h"
//#include "quadknob.h"
#include "keypad.h"
#include "smc_queue.h"

///////////////////////////
// Test -without input, the expected output = ball goes around the board until it hits a wall that's not blocked by a paddle
// Without_Input - Works!
//#define TEST_WITHOUT_INPUT

// Test - with input:
// 1. Paddles move up and down within the bounds of the board
// 2. Ball can move to all legal positions and all illegal positions & game ending positions end the game

extern volatile int32_t timer_isr_countdown; // Required to control timing
const int game_board_size = CHECKS_WIDE; // Provided for extern

void ram_health(uint16_t dummy_var, uint16_t pattern){
	// DEBUGGING PHASE: LOCK UP THE PROGRAM if RAM is corrupted.
	if (dummy_var != pattern){
		while(1);
	}
}


void pong_main(void){
	const int32_t timer_isr_500ms_restart = 500;
	const int32_t timer_isr_2000ms_restart = 2000;

	// INITIALIZE THE GAME
	// Construct the model "game" object:
	static pong_board my_game;
	volatile uint16_t ram_dummy_1 = MEMORY_BARRIER_1;
	pong_game_init(&my_game);

	// Construct IPC REAMKE FOR KEYPAD & PONG
	Smc_queue l_shuffle_q;		//queue for storing input for the left paddle
	Smc_queue r_shuffle_q;		//queue for storing input for the right paddle
	volatile uint16_t ram_dummy_2 = MEMORY_BARRIER_2;
	smc_queue_init(&l_shuffle_q);
	smc_queue_init(&r_shuffle_q);

	// Input object initialization
//	keypad user_keypad;
	volatile uint16_t ram_dummy_3 = MEMORY_BARRIER_3;
//	quadknob_init(&user_knob_1);
	initbuttons();				//Keypad func that initializes the button lines

	// Output object
	// Block all interrupts while initializing - initial protocol timing is critical.
	__disable_irq();
	display_init();
	__enable_irq();

	// Welcome screen = checkerboard for 2 seconds.
	timer_isr_countdown = timer_isr_2000ms_restart;
	display_checkerboard();
	while (timer_isr_countdown > 0){}
	timer_isr_countdown = timer_isr_500ms_restart;
	display_blank();

	// OPERATE THE GAME
	int32_t prior_timer_countdown = timer_isr_countdown;

	while(1){
		ram_health(ram_dummy_1, MEMORY_BARRIER_1);
		ram_health(ram_dummy_2, MEMORY_BARRIER_2);
		ram_health(ram_dummy_3, MEMORY_BARRIER_3);

	// ASSERT TIMER COUNTDOWN IN RANGE
		if ((timer_isr_countdown > timer_isr_500ms_restart)||
				(timer_isr_countdown < 0)){
			display_checkerboard();
			while(1);
		}

#ifndef TEST_WITHOUT_INPUT
		// Check for user input every 1 ms & paint one block of the display.
		if (prior_timer_countdown != timer_isr_countdown ){
			prior_timer_countdown = timer_isr_countdown;
			// If time changed, about 1 ms has elapsed.
			// Once each 1 ms, read input pins from KEYPAD

			//Initialize the command packet for input
			Q_data l_command_packet;
			Q_data r_command_packet;
			//Paddle_L check controls
			int control_L_paddle = 0;
			int control_R_paddle = 0;
			static int old_l_control = 0;
			static int old_r_control = 0;
			control_L_paddle = check_column1();
			control_R_paddle = check_column2();
			if (control_L_paddle != old_l_control){
				switch(control_L_paddle){
				case 1:
					l_command_packet.movement = DOWN;
					l_shuffle_q.put(&l_shuffle_q, &l_command_packet);
					break;
				case 2:
					l_command_packet.movement = UP;
					l_shuffle_q.put(&l_shuffle_q, &l_command_packet);
					break;
				default:
					l_command_packet.movement = STAY;
					l_shuffle_q.put(&l_shuffle_q, &l_command_packet);
					break;
				}
				paddle_shuffle(&my_game.paddle_L, &l_shuffle_q);
				old_l_control = control_L_paddle;
			}

			//Paddle_R check controls
			if(control_R_paddle != old_r_control){
				switch(control_R_paddle){
				case 1:
					r_command_packet.movement = DOWN;
					r_shuffle_q.put(&r_shuffle_q, &r_command_packet);
					break;
				case 2:
					r_command_packet.movement = UP;
					r_shuffle_q.put(&r_shuffle_q, &r_command_packet);
					break;
				default:
					r_command_packet.movement = STAY;
					r_shuffle_q.put(&r_shuffle_q, &r_command_packet);
					break;
				}
				paddle_shuffle(&my_game.paddle_R, &r_shuffle_q);
				old_r_control = control_R_paddle;
			}

		// ASSERT HEADING IS VALID
			while ((my_game.ball.dir != N)&&
					(my_game.ball.dir != NE)&&
					(my_game.ball.dir != E)&&
					(my_game.ball.dir != SE)&&
					(my_game.ball.dir != S)&&
					(my_game.ball.dir != SW)&&
					(my_game.ball.dir != W)&&
					(my_game.ball.dir != NW));
			incremental_show_game(&my_game, false);
		}
		if (timer_isr_countdown <= 0) {
			// Move every 500 ms
			timer_isr_countdown = timer_isr_500ms_restart;
			pong_periodic_play(&my_game);
		}
#endif
#ifdef TEST_WITHOUT_INPUT
		// This block of code tests if the game works by spoofing paddle shuffle inputs.
		// L paddle moves up, down, then up, and loops back. R paddle stays put.
		static int shuffles = 0;
		// Normally "check for user input every 1 ms & show" - here just update display
		if (prior_timer_countdown != timer_isr_countdown ){
			prior_timer_countdown = timer_isr_countdown;
			incremental_show_game(&my_game, false);
		}
		if (timer_isr_countdown <= 0) {
			// Move and animate every 500 ms
			timer_isr_countdown = timer_isr_500ms_restart;

			//Initialize input command packet states
			Q_data command_packet;
			switch(shuffles){
			case 0:
				command_packet.movement = UP;
				shuffles++;
				l_shuffle_q.put(&l_shuffle_q, &command_packet);
				paddle_L_shuffle(&my_game, &l_shuffle_q);
				pong_periodic_play(&my_game);
				break;
			case 1:
				command_packet.movement = UP;
				shuffles++;
				l_shuffle_q.put(&l_shuffle_q, &command_packet);
				paddle_L_shuffle(&my_game, &l_shuffle_q);
				pong_periodic_play(&my_game);
				break;
			case 2:
				command_packet.movement = DOWN;
				shuffles++;
				l_shuffle_q.put(&l_shuffle_q, &command_packet);
				paddle_L_shuffle(&my_game, &l_shuffle_q);
				pong_periodic_play(&my_game);
				break;
			case 3:
				command_packet.movement = DOWN;
				shuffles = 0;
				l_shuffle_q.put(&l_shuffle_q, &command_packet);
				paddle_L_shuffle(&my_game, &l_shuffle_q);
				pong_periodic_play(&my_game);
				break;
			default:		//Error handler, throws it back to state 0
				shuffles = 0;
				pong_periodic_play(&my_game);
				break;
			}
		}
#endif
	}
}

// Time-wasting counter as delay (about 2 seconds).
// for (uint32_t i = 0; i < 2345678; i++){;} // pause several sec b4 repeat all
