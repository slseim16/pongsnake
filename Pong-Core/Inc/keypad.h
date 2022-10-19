/*
 * keypad.h
 *
 *  Created on: Oct 10, 2022
 *      Author: slseim16
 */
#include "main.h"
#include "stdbool.h"
#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

typedef
	struct keypad_struct {
	// Attributes
	int button1;
	int button3;
	int button7;
	int button9;
}keypad;
void initbuttons();
int check_column1();
int check_column2();
#endif /* INC_KEYPAD_H_ */
