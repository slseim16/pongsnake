/*
 * smc_queue_struct.c
 *
 *  Created on: Jul 15, 2022
 *      Author: carrolls
 */

#include "smc_queue.h"
#include "snake_enums.h"

Smc_queue* smc_queue_init(Smc_queue* q){
	q->head = 0;
	q->tail= 0;
	q->cap = SMC_Q_BUFSIZE;
	q->burden = 0;
	q->put = &(smc_queue_put);
	q->get = &(smc_queue_get);
	q->peek = &(smc_queue_peek);
	return q;
}


bool smc_queue_put(Smc_queue *q, const Q_data *msg){
	bool success = false;
	// FIRST check if there is room in the queue
	if (q->burden >= q->cap) success = false;
	else {
		 q-> burden += 1;
		 q->buffer[q->tail] = *msg;
		 q->tail += 1;
		 success = true;
	}
	return success;
}

bool smc_queue_get(Smc_queue *q, Q_data  *msg){
	bool success = false;
	// FIRST check if there is data in the queue
	if (q->burden == 0) success= false;

	else {
	    // Get message from front
		*msg = q->buffer[0];
		// Shuffle others forward
		for (int n = 0; n < (q->tail - 1); n++){
		   q->buffer[n] = q->buffer[n+1];
		}
		// Bookkeeping
		q-> tail -= 1;
		q->burden -= 1;
		success = true;
	}
	return success;
}


bool smc_queue_peek(const Smc_queue *q, Q_data  *msg){
	bool success = false;
	// FIRST check if there is data in the queue
	if (q->burden == 0) success = false;
	// If YES - copy data but do not modify anything.
	else {
		*msg = q->buffer[0];
		success = true;
	}
	return success;
}
