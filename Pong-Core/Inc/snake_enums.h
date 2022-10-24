/*
 * snake_turn.h
 *
 *  Created on: Aug 4, 2022
 *      Author: carrolls
 */

#ifndef INC_SNAKE_TURN_H_
#define INC_SNAKE_TURN_H_

enum snake_direction {CRAWL_X_PLUS, CRAWL_X_MINUS, CRAWL_Y_PLUS, CRAWL_Y_MINUS};        //Remake for pong
enum Twist {QUADKNOB_CW, QUADKNOB_CCW, QUADKNOB_STILL};                                 //Potentially unnecessary, investigate QUADKNOB further
enum DED {DED_LOW, DED_RISING, DED_HIGH, DED_FALLING, DED_UNDEF};
enum DED_Hidden {DED_HIDDEN_00, DED_HIDDEN_01, DED_HIDDEN_11, DED_HIDDEN_10, DED_HIDDEN_XX};
enum snake_compass_dir {SNAKE_COMPASS_N, SNAKE_COMPASS_S, SNAKE_COMPASS_E, SNAKE_COMPASS_W, SNAKE_COMPASS_0, SNAKE_COMPASS_FRUIT_HERE};     //Not necessary and no need to remake?
#endif /* INC_SNAKE_TURN_H_ */
