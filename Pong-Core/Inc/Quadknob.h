/*
 * Quadknob.h
 *
 *  Created on: Jul 17, 2022
 *      Author: carrolls
 *
 *   Contains DebouncedEdgeDetector and QuadKnob
 */

#ifndef INC_QUADKNOB_H_
#define INC_QUADKNOB_H_

#include "main.h"
#include "stdbool.h"
#include "snake_enums.h"


// "Class" DebouncedEdgeDetector///////////////////////////////////////////////
// Attributes & method pointers for DebouncedEdgeDetector
typedef
		struct DebouncedEdgeDetector_struct {
		// Attribute
			enum DED_Hidden prior_state;
			enum DED output;
			// Method pointer
			void (*update) (struct DebouncedEdgeDetector_struct* self, bool new_bit);
			enum DED (*get)(const struct DebouncedEdgeDetector_struct* self);
} DebouncedEdgeDetector;

// Methods for DebouncedEdgeDetector
void DebouncedEdgeDetector_init(DebouncedEdgeDetector *self);
void DebouncedEdgeDetector_update (DebouncedEdgeDetector *self, bool new_bit);
enum DED DebouncedEdgeDetector_get(const DebouncedEdgeDetector* self);
///////////////////////////////////////////////////////////////////////////////


// "Class" QuadKnob////////////////////////////////////////////////////////////
// Types for QuadKnob

// Attributes & method pointers for QuadKnob
typedef
	struct QuadKnob_struct {
	// Attributes
		DebouncedEdgeDetector signal_A;
		DebouncedEdgeDetector signal_B;
		enum Twist turning;
	// Method Pointers
		void (*update)(struct QuadKnob_struct *self, bool a, bool b);
		enum Twist (*get)(const struct QuadKnob_struct *self);
}QuadKnob;

// Methods for QuadKnob
enum Twist QuadKnob_get(const QuadKnob *self);
void quadknob_update(QuadKnob *self, bool a_is_active, bool b_is_active);
void quadknob_init(QuadKnob *self);

///////////////////////////////////////////////////////////////////////////////


#endif /* INC_QUADKNOB_H_ */
