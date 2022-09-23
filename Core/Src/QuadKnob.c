/*
 * QuadKnob.c
 *
 * C-language class-like definition
 * Purpose is simple: Given TWO logic values (active/inactive)
 * (1) Debounce them -- a stateful operation
 * (2) Interpret them as turning clockwise, turning CCW,
 *     or not turning, based on the  interpretation of the
 *     two logic signals as quadrature encoded pins.
 */

#include <quadknob.h>
#include "main.h"


void
    DebouncedEdgeDetector_init(DebouncedEdgeDetector *self){
	    self->update = &DebouncedEdgeDetector_update;
	    self->get = &DebouncedEdgeDetector_get;
	    self->output = DED_UNDEF;
		self->prior_state = DED_HIDDEN_XX;
}

// The debouncing & edge-detecting state machine.
// We need a pair for these for each "Quad encoder" knob -- each decoding
// either pin A or pin B. (Another function will compare A-high and B-rising,
// etc. to decipher user intent).
// This state machine will report RISING or FALLING only after 2 consecutive
// readings of the polarity opposite the old state - then emit the edge notice.
void DebouncedEdgeDetector_update (DebouncedEdgeDetector *self, bool new_bit){
	enum DED outVal = DED_UNDEF;
	switch (self->prior_state){
	case DED_HIDDEN_XX:
		if (new_bit == false){
			self->prior_state = DED_HIDDEN_00;
			outVal = DED_LOW;
		} else {
			self->prior_state = DED_HIDDEN_11;
			outVal = DED_HIGH;
		}
		break;
	case DED_HIDDEN_00:
		if (new_bit == false){
			// prior_state stays LOW
			outVal = DED_LOW;
		}
		else{
			self->prior_state = DED_HIDDEN_01;
			outVal = DED_LOW;
		}
		break;
	case (DED_HIDDEN_01):
	    if (new_bit == false){
	    	self->prior_state = DED_HIDDEN_00; // False alarm
	    	outVal = DED_LOW;
	    } else {
	    	self->prior_state = DED_HIDDEN_11; // Confirmed: 0-to-1
	    	outVal = DED_RISING;
	    }
	break;
	case (DED_HIDDEN_11):
			if (new_bit == false){
				self->prior_state = DED_HIDDEN_01;
				outVal = DED_HIGH;
			} else {
				// prior_state stays high
				outVal = DED_HIGH;
			}
	break;
	case (DED_HIDDEN_10):
	    if (new_bit == false){
	    	self->prior_state = DED_HIDDEN_00; // Confirmed
	    	outVal = DED_FALLING;
	    } else {
	    	self->prior_state = DED_HIDDEN_11; // False alarm
	    	outVal = DED_HIGH;
	    }
	break;
	default:
		self->prior_state = DED_HIDDEN_XX;
		outVal = DED_UNDEF;
	}
    self->output = outVal;
}


enum DED DebouncedEdgeDetector_get(const DebouncedEdgeDetector *self){
	return self->output;
}

void
    quadknob_init(QuadKnob *self) {
	// Initialize all methods:
	self->update = &quadknob_update;
	self->get = &QuadKnob_get;

	// Initialize all attributes.
	self->turning = QUADKNOB_STILL;
	// The edge-detector attributes are objects, so call their init functions.
	// In C++, the compiler would do this for us, but in C we must request it.
	DebouncedEdgeDetector_init(&(self->signal_A));
	DebouncedEdgeDetector_init(&(self->signal_B));
}


void quadknob_update(QuadKnob *self, bool a_is_active, bool b_is_active){
	enum Twist result = QUADKNOB_STILL;
	// Awkward syntax for C below!
	// To calculate the direction that a quadrature-encoded knob is turning --
	// where the possibilities are clockwise, counter clockwise, or not-turning
	// (a.k.a. "still"), we need to check for the status of two pins, here called
	// pin A and pin B. Since the encoder is bouncy, we will call a state machine
	// for each pin.

	// We have two similar state machines for the two pins of a quad-knob, and we'll
	// update each of them to learn which one is "rising first".
	// NOTE -  According to the data sheet for the part, if A rises while B is
	// still low, then the knob just turned one click CW. But is B rises while
	// A is still low, then the knob is turning counter clockwise.
	//
	// This is complicated: we need to compare the output from two similar state
	// machines wired to different pins - so it can get confusing. C does not help
	// a whole lot at keeping this straight so I will comment out my idea and my
	// C syntax in a few paragraphs:
	//
	// The state-machine function for each pin is stored with the pin's data (that's
	// good -- it is the policy of "encapsulation" that makes OOP work well).
	//
	// TO START: In the function declaration for QuadKnob_update, the variable "self"
	// is a pointer to a single QuadKnob. Every QuadKnob has two attributes called
	// signal_A and signal_B. We access them as (self->signal_A) and (self->signal_B).
	// These are similar state machines.
	//
	// It is easier to make a holder variable for each pointer - call them DED_A
	// and DED_B for the two DebouncedEdgeDetectors.
	//
	// Then we call on each machine to update itself based on the logic-level input
	// x_is_active.

	DebouncedEdgeDetector* DED_A = &(self->signal_A);
	DED_A->update(DED_A, a_is_active);
	enum DED edge_a = DED_A->get(DED_A);

	DebouncedEdgeDetector* DED_B = &(self->signal_B);
	DED_B->update(DED_B, b_is_active);
	enum DED edge_b = DED_B->get(DED_B);

	if ((edge_a == DED_RISING) && (edge_b == DED_LOW))
			result = QUADKNOB_CCW;
	else if ((edge_a == DED_LOW) && (edge_b == DED_RISING))
			result = QUADKNOB_CW;
	else
		result = QUADKNOB_STILL;

	self->turning = result;
}


enum Twist QuadKnob_get(const  QuadKnob *self){
	return self->turning;
}
