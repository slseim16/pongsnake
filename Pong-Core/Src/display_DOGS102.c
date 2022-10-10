// carrolls@trine.edu 2022.7.22
// Now part of G071_Demo_Snake
///////////////////////////////////////////////
// DogsA - initializes and displays a checkerboard of 8x8 pixel
// squares alternating foreground/background color on a DOGS102
// 64x102 pixel graphic display ($17).
// By Sean Carroll, Trine U, v. 2022.6.22 = Nucleo-G071 SPI Master.
// Purpose = Demo project - uses CubeMX and HAL.
// NOTE - SPI1 appears to be used by the debugger.
// Software-Defined Requirements:
//     SPI.SCK out on PB10, SPI.MOSI on PC3,
//     GP_O.nReset out @ PA_0, GPO.nCS @ PD_9, GPO.nC_D @ PA_1
//  TESTED as of 2022.6.21
// POWER = 3.3 V and GND.
//////////////////////////////////////////////
// /*WIRING:
		// 15|---||-GND
		// 16|-------,
		// 17|--||,  |
		// 18|----'  |
		// 19|----||-'
		// 20|--GND
		// 21|--GND
		// 22|--3.3v
		// 23|--3.3v
		// 24|--SDA-- (NucleoG071/PC3)
		// 25|--SCK-- (NucleoG071/PB10)
		// 26|--CD--- (NucleoG071/PA1)
		// 27|--RST-- (NucleoG071/PA0)
		// 28|--CS0-- (NucleoG071/PD9 - nSS)
// */


// This display driver uses the pins defined in an .ioc file
// created with STM32CubeMX.
#include <pong_gameplay.h> // To learn "CHECKS_WIDE"
#include "main.h"
#include "display_DOGS_102.h"

/////////////////////////////////////////////////////////////////////////////
// Hand-written code that Cube won't help with
/////////////////////////////////////////////////////////////////////////////

extern  SPI_HandleTypeDef hspi2; //TUTORIAL ...
// I wrote this line to borrow the hspi2 struct which was defined
// for me by STM32CubeMX in MAIN.C. I have to, or the the calls
// to HAL_SPI_Transmit(&hspi2 ...) won't build.




static void spi_msg_out(DOGS_packet pkt){
	const int16_t twelve_us = 20;
	const uint32_t SPI_timeout = 100;
	// Pulse the CS line high to mark change-of-packet
	// GPIOD->ODR |= GPIO_ODR_OD9;
	//  -- or, in HAL notation -- //
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);  // * GPIO_PIN_9 == 1<<9
	for (int32_t i = 0; i<twelve_us; i++)
	   {;}                   // just counting to waste time for about 12 us
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);

	// Assert the Command/Data line
	if (pkt.nCD){
       GPIOA->ODR |= GPIO_ODR_OD1;
	}else{
        GPIOA->ODR &=~ GPIO_ODR_OD1;
    }

	// Send one packet (a byte).
	// Use the HAL.
	// SPI1: Send 1 byte of pkt.byte, and timeout only after 100 ticks
	// EXPECT ABOUT 16 us of delay from PD_6 low to SCK action.
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&pkt.byte, 1, SPI_timeout);
}



const DOGS_packet init_stream[] =
{   {0, 0x40},  // Display processor boot! Startline = 0
    {0, 0xa1},  // "SEG-reversed" for view-from-bottom
	{0, 0xc0},  // COM-normal for view=from-bottom
	{0, 0xa4},  // Pixels show RAM
	{0, 0xa6},  // Foreground dark, background clear
	{0, 0xa2},  // Set "bias" to 1/9 = typical LCD tech
	{0, 0x2f},  // Power booster ON
	{0, 0x27},  // Internal Resistor Ratio
	{0, 0x81},  // Set volume = contrast
	{0, 0x02},
	{0, 0xaf}  // Display Active!
}; // Send another "!" shape

void display_init(void){

	const uint32_t five_ms = 40000;
	const int init_stream_length = sizeof(init_stream)/ sizeof(init_stream[0]);
	// - LET CUBEMX CONFIGURE REGISTERS // config_periph();
	// Hardware reset first:
	HAL_GPIO_WritePin(DOGS_reset_GPIO_Port, DOGS_reset_Pin, GPIO_PIN_RESET);
	// Or use the bit-banging method to do the same thing:
	GPIOA->ODR &=~ GPIO_ODR_OD0; // nReset Low

	GPIOA->ODR |= GPIO_ODR_OD1;  // nCS high -

	for (uint32_t i = 0; i < five_ms; i++){;} // pause 5 ms

	GPIOA->ODR |= GPIO_ODR_OD0; // nReset High

	for (uint32_t i = 0; i < five_ms; i++){;} // pause 5 ms

	GPIOA->ODR &= ~GPIO_ODR_OD1;  // nCS low
	for (uint32_t i = 0; i < five_ms; i++){;} // pause 5 ms

	// Initialize the DOGS with a 11-command stream, then send a simple image to plot
	for (uint16_t init_index = 0; init_index < init_stream_length; init_index++){
        spi_msg_out(init_stream[init_index]);
    }
}



void display_blank(void){
	const int display_width = DISPLAY_WIDTH;
	const DOGS_packet white = {1, 0x00};
	for (uint16_t pg = 0; pg < CHECKS_WIDE; pg++){
		DOGS_packet page_set = {0, 0xb0|pg};
		for (uint16_t col = 0; col < display_width; col++){
			spi_msg_out(page_set);
			DOGS_packet col_set[2] = {{0, (col &0x0f)}, {0, 0x10|(col>>4)}};
			spi_msg_out(col_set[0]);
			spi_msg_out(col_set[1]);
			spi_msg_out(white);
		}
	}
}


void display_checkerboard(void){
	const int checkerboard_squares = CHECKS_WIDE;
	const int checkerboard_pixels_wide = PIXELS_PER_CHECK * CHECKS_WIDE;
	const int display_width = DISPLAY_WIDTH;
	const DOGS_packet black = {1, 0xFF};
	const DOGS_packet white = {1, 0x00};
	for (uint16_t pg = 0; pg < checkerboard_squares; pg++){
		DOGS_packet page_set = {0, 0xb0|pg};
		for (uint16_t col = 0; col < display_width; col++){
			spi_msg_out(page_set);

			DOGS_packet col_set[2] = {{0, (col &0x0f)}, {0, 0x10|(col>>4)}};
			spi_msg_out(col_set[0]);
			spi_msg_out(col_set[1]);
			if ((col < checkerboard_pixels_wide) &
					!((pg+col/PIXELS_PER_CHECK) & 0x01))
			{ // BIT-BANGING for evenness is QUICK
				spi_msg_out(black);
			}else{
				spi_msg_out(white);
			}
		}
	}
}


void display_snake_board(int8_t board[CHECKS_WIDE][CHECKS_WIDE]){
	const int checkerboard_squares = CHECKS_WIDE;
	const int checkerboard_pixels_wide = PIXELS_PER_CHECK * CHECKS_WIDE;
	const int display_width = DISPLAY_WIDTH;
	const DOGS_packet black = {1, 0xFF};
	const DOGS_packet white = {1, 0x00};
	for (uint16_t pg = 0; pg < checkerboard_squares; pg++){
		DOGS_packet page_set = {0, 0xb0|pg};
		spi_msg_out(page_set);
		for (uint16_t col = 0; col < display_width; col++){
			DOGS_packet col_set[2] = {{0, (col &0x0f)}, {0, 0x10|(col>>4)}};
			spi_msg_out(col_set[0]);
			spi_msg_out(col_set[1]);
			if ((col < checkerboard_pixels_wide) &
					(board[col/PIXELS_PER_CHECK][pg] != 0))
			{
				spi_msg_out(black);
			}else{
				spi_msg_out(white);
			}
		}
	}
}


static void display_paint_square(uint8_t l_to_r, uint8_t t_to_b, DOGS_packet p){
	if ((l_to_r > (DISPLAY_WIDTH/PIXELS_PER_CHECK))||(t_to_b > CHECKS_WIDE)) {
		display_checkerboard();
	}
	else {
		DOGS_packet page_set = {0, 0xb0|t_to_b};
		spi_msg_out(page_set);
		for (uint16_t n = 0; n < PIXELS_PER_CHECK; n++){
			uint8_t col = l_to_r * PIXELS_PER_CHECK + n;
			DOGS_packet col_set[2] = {{0, (col &0x0f)}, {0, 0x10|(col>>4)}};
			spi_msg_out(col_set[0]);
			spi_msg_out(col_set[1]);
			spi_msg_out(p);
		}
	}
}

void display_white_square(uint8_t l_to_r, uint8_t t_to_b){
	const DOGS_packet white = {1, 0x00};
	display_paint_square(l_to_r, t_to_b, white);
}

void display_dark_square(uint8_t l_to_r, uint8_t t_to_b){
	const DOGS_packet black = {1, 0xFF};
	display_paint_square(l_to_r, t_to_b, black);
}
