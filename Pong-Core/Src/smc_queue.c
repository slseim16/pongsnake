/*
 * smc_queue_struct.c
 *
 *  Created on: Jul 15, 2022
 *      Author: carrolls
 *  Edited by: brhinman20
 */

#include <pong_enums.h>
#include "smc_queue.h"

Smc_queue* smc_queue_init(Smc_queue* q){
	q->head = 0;
	q->tail= 0;
	q->cap = SMC_Q_BUFSIZE-1; // Must be -1 because addresses wrap from 9 to 0 in a 10 size queue
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
		 //Check if tail needs to wrap from address 9 to address 0
		 if (q->tail < q->cap) {
			 q->tail += 1;
		 }
		 //If wrap needed, wrap tail to 0
		 else {
			 q->tail = 0;
		 }
		 success = true;
	}
	return success;
}

bool smc_queue_get(Smc_queue *q, Q_data  *msg){
	bool success = false;
	// FIRST check if there is data in the queue
	if (q->burden == 0) success= false;

	else {
	    // Get message from head
		*msg = q->buffer[q->head];
		// Bookkeeping
		// Check if head needs to wrap from address 9 to address 0
		if (q->head < q->cap) {
			q->head += 1;
		}
		//If wrap needed, wrap tail to 0
		else {
			q->head = 0;
		}
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
		*msg = q->buffer[q->head];
		success = true;
	}
	return success;
}
