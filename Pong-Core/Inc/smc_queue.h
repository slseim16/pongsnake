/*
 * smc_queue_struct.h
 *
 *  Created on: Jul 15, 2022
 *      Author: carrolls
 *      Edited: seand
 */


#include <pong_enums.h>
#include <stddef.h> // size_t
#include <stdint.h>
#include <stdbool.h>

#ifndef INC_SMC_QUEUE_STRUCT_H_
#define INC_SMC_QUEUE_STRUCT_H_

#define SMC_Q_BUFSIZE 10

// To make a queue that is easy to revise to hold any type
// this file defines the contents of all queues as Q_data
// and defines that type at only one point: here.
// If you want to pass messages other than this union, you
// do not need to change any code in smc_queue_struct.h or .c
// except at this point.
// I am showing off a feature by using a UNION, but replacing
// if with "typedef int32_t Q_data;" would be simpler and safer.

// REQUIRED TO USE

typedef union {
		int int_val;
		float float_val;
//		int8_t board[CHECKS_WIDE][CHECKS_WIDE];		//Potential for board to be passed via queues
		enum move movement;							//Movement enum for left paddle
} Q_data;

typedef struct smc_queue_struct {
	// ATTRIBUTES
	size_t cap;						//Max no of things in the queue
	size_t burden;					//How many things are in the queue
	size_t tail;					//What's at the start of the queue
	size_t head;					//What's at the end of the queue
	Q_data buffer[SMC_Q_BUFSIZE];

	// METHODS
	bool (*put)(struct smc_queue_struct *q, const Q_data *msg);
	bool (*peek)(const struct smc_queue_struct *q, Q_data *msg);
	bool (*get)(struct smc_queue_struct *q,  Q_data *msg);
} Smc_queue;

Smc_queue* smc_queue_init( Smc_queue* Q);
bool smc_queue_put(Smc_queue *q, const Q_data *msg);
bool smc_queue_peek(const Smc_queue *q, Q_data *msg);
bool smc_queue_get(Smc_queue *q, Q_data *msg);




#endif /* INC_SMC_QUEUE_STRUCT_H_ */
