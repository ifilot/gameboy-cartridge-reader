#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <string.h>

#include "uart.h"

#define UART_BAUD_RATE 512000

#define LED_RD_ON		PORTD |= (1 << 2)
#define LED_RD_OFF		PORTD &= ~(1 << 2)
#define LED_WR_ON		PORTD |= (1 << 3)
#define LED_WR_OFF		PORTD &= ~(1 << 3)
#define RAM_HIGH		PORTD |= (1 << 4)
#define RAM_LOW			PORTD &= ~(1 << 4)
#define READ_HIGH		PORTD |= (1 << 5)
#define READ_LOW		PORTD &= ~(1 << 5)
#define WRITE_HIGH		PORTD |= (1 << 6)
#define WRITE_LOW		PORTD &= ~(1 << 6)

#define AUDIO_HIGH		PORTE |= (1 << 1)
#define AUDIO_LOW	    PORTE &= ~(1 << 1)
#define RES_HIGH		PORTE |= (1 << 2)
#define RES_LOW			PORTE &= ~(1 << 2)

// board id and compile time statistics
static const char board_id[17] = {'G','B','C','R','-','8','5','1','5','-','V','2','.','0','.','0','\0'};
static const char cdate[17] = __DATE__;
static const char ctime[17] = __TIME__;

// command storage
char instruction[9];    // stores single 8-byte instruction
uint8_t inptr = 0;      // instruction pointer

// forward declarations6
void init();
void write_board_id();
void compile_time();
void parse_instructions();
void read_header(void);
void set_upper_address(uint8_t);
void set_lower_address(uint8_t);
void set_address(uint16_t);
void read_sector(uint16_t addr);

int main(void) {
	// set default values for pins
	init();

    // initialize UART library
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    sei();	// disable interrupts (UART is interrupt driven)

	uint16_t c;
    for (;;) {
	    c = uart_getc(); // try to grab a character
	    if (c & UART_NO_DATA) {
		    // do nothing if there is no data
		    } else {
		    if (c & UART_FRAME_ERROR || c & UART_OVERRUN_ERROR || c & UART_BUFFER_OVERFLOW) {
			    // something went wrong, discard the character
			    } else {
			    // only capture alphanumerical data
			    if((c >= 48 && c <= 57) || (c >= 65 && 90)) {
				    instruction[inptr] = c;
				    inptr++;
			    }
			    
			    if(inptr == 8) {
				    parse_instructions();
				    inptr = 0;
			    }
		    }
	    }
    }
}

void init() {
	DDRA = 0xFF;
	PORTA = 0x00;
	DDRB = 0xFF;
	PORTB = 0x00;
	DDRC = 0x00;
	PORTC = 0x00;
	DDRD = DDRD | 0b11111100;
	DDRE = DDRE | 0b00000111;
	LED_RD_OFF;
	LED_WR_OFF;
	READ_HIGH;
	WRITE_HIGH;
	RES_HIGH;
	RAM_HIGH;
	AUDIO_HIGH;
}

/*
 * @brief echo to command to confirm that it was successfully received
 */
void echo_command() {
	for(uint8_t i=0; i<8; i++) {
		uart_putc(instruction[i]);
	}
	
	uart_flush();
}

/*
 * @brief Check if part of two strings are equal
 * @param original string
 * @param reference string
 * @param offset in original string
 * @param length to read
 */
bool check_command(const char* cmd, const char* ref, uint8_t offset, uint8_t length) {
	for(uint8_t i=0; i<length; i++) {
		if(cmd[i+offset] != ref[i]) {
			return false;
		}
	}
	return true;
}

/*
 * @brief convert 4 HEX bytes of instruction to 16 bit unsigned integer
 * @param command word
 * @param offset in command
 */
uint16_t get_uint16(const char* instruction, uint8_t offset) {
	char buffer[5];
	memcpy(buffer, &instruction[offset], 4);
	buffer[4] = '\0';
	return strtoul(buffer, NULL, 16);
}

/*
 * @brief convert 2 HEX bytes of instruction to 8 bit unsigned integer
 * @param command word
 * @param offset in command
 */
uint8_t get_uint8(const char* instruction, uint8_t offset) {
	char buffer[3];
	buffer[0] = instruction[offset];
	buffer[1] = instruction[offset+1];
	buffer[2] = '\0';
	return strtoul(buffer, NULL, 16);
}

/*
 * @brief parse instructions received over serial
 */
void parse_instructions() {
	echo_command();
	
	if(check_command(instruction, "READINFO", 0, 8)) {
		write_board_id();
		return;
	} else if(check_command(instruction, "READHDR0", 0, 8)) {
		read_header();
		return;
	} else if(check_command(instruction, "COMPTIME", 0, 8)) {
		compile_time();
		return;
	} else if(check_command(instruction, "RDBK", 0, 4)) {
		read_sector(get_uint16(instruction, 4));
		return;
	}
}

/*
 * @brief Write board ID to serial
 */
void write_board_id() {
	for(uint8_t i=0; i<16; i++) {
		uart_putc(board_id[i]);
	}
}

/*
 * @brief Yield compile time
 */
void compile_time() {
	for(uint8_t i=0; i<16; i++) {
		uart_putc(cdate[i]);
	}
	for(uint8_t i=0; i<16; i++) {
		uart_putc(ctime[i]);
	}
}

/*
 * @brief Set upper eight bits of the address
 * @param address
 */
void set_upper_address(uint8_t addr) {
	PORTA = addr;
}

/*
 * @brief Set lower eight bits of the address
 * @param address
 */
void set_lower_address(uint8_t addr) {
	PORTB = addr;
}

/*
 * @brief Set address
 * @param address
 */
void set_address(uint16_t addr) {
	PORTA = addr >> 8;		// upper byte
	PORTB = addr & 0xFF;	// lower byte
}

/*
 * @brief Read cartridge header
 *
 * Read the first 0x150 bytes of the cartridge
 */
void read_header() {
	LED_RD_ON;
	
	for(uint16_t i=0; i<0x150; i++) {
		set_address(i);
		READ_LOW;	// note; one NOP for 8 MHz, two for 16 MHz
		asm volatile("nop");
		uart_putc(PINC);
		READ_HIGH;
	}
	
	LED_RD_OFF;
}

/*
 * @brief Read a sector of 0x1000 bytes of the cartridge
 * @param starting address
 *
 * Read 0x1000 bytes starting at address addr; note that
 * the lower 12 bits of the address need to be zero
 * 
 */
void read_sector(uint16_t addr) {
	for(uint8_t j=0; j<0x10; j++) {
		set_upper_address((uint8_t)(addr >> 8) + j);

		LED_RD_ON;
		
		uint8_t i = 0;
		do {
			set_lower_address(i);
			READ_LOW;	// note; one NOP for 8 MHz, two for 16 MHz
			asm volatile("nop");
			uart_putc(PINC);
			READ_HIGH;
		} while(i++ != 255);
		
		uart_flush();
		LED_RD_OFF;
	}
	
	LED_RD_OFF;
}