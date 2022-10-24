/*
 * keypad.c
 *
 *  Created on: Oct 10, 2022
 *      Author: slseim16
 */
#include <keypad.h>
#include "main.h"

//Changed ReadPin lines from == 0 to == 1
void initbuttons(){
	GPIOC->ODR |= 1<<8;
}
int check_column1(){
	int updown;
	updown=0;
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11)==0){
		updown=1;
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12)==0){
		updown=2;
	}
	GPIOC->ODR &=~(1<<8);
	GPIOC->ODR |= (1<<6);
	return updown;
}
int check_column2(){
	int updown=0;
	if(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_11)==0){
		updown=1;
	}
	else if(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_12)==0){
		updown=2;
	}
	GPIOC->ODR &=~(1<<6);
	GPIOC->ODR |= (1<<8);
	return updown;
}
