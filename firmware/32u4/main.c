#include <avr/io.h>
#include <avr/eeprom.h> 
#include <util/delay.h>
#include <stdlib.h>

#include "VirtualSerial.h"

// list of defines for easy toggling of pins
#define READ_HIGH		PORTB |= (1 << 5)
#define READ_LOW		PORTB &= ~(1 << 5)
#define WRITE_HIGH		PORTB |= (1 << 6)
#define WRITE_LOW		PORTB &= ~(1 << 6)
#define WRITE_PULSE     {WRITE_LOW;WRITE_HIGH;}    // triggered on falling edge   
#define SRAM_HIGH       PORTE |= (1 << 6)
#define SRAM_LOW        PORTE &= ~(1 << 6)
#define RESET_HIGH      PORTF |= (1 << 7)
#define RESET_LOW		PORTF &= ~(1 << 7)
#define AUDIO_HIGH      PORTF |= (1 << 6)
#define AUDIO_LOW       PORTF &= ~(1 << 6)
#define AUDIO_PULSE     {AUDIO_LOW; AUDIO_HIGH;}    // triggered on falling edge   
#define CPL_HIGH		PORTF |= (1 << 0)
#define CPL_LOW			PORTF &=  ~(1 << 0)
#define CPL_TOGGLE		{CPL_HIGH; CPL_LOW;}		// triggered on rising edge
#define CPH_HIGH		PORTF |= (1 << 1)
#define CPH_LOW			PORTF &=  ~(1 << 1)
#define CPH_TOGGLE		{CPH_HIGH; CPH_LOW;}		// triggered on rising edge
#define CRST_HIGH		PORTB |= (1 << 7)
#define CRST_LOW		PORTB &= ~(1 << 7)
#define CRST_PULSE      {CRST_HIGH; CRST_LOW;}		// triggered on rising edge
#define PINS_INPUT		{DDRD = 0x00; PORTD = 0x00;}
#define PINS_OUTPUT     DDRD = 0xFF
#define WAIT2			{asm volatile("nop");asm volatile("nop");}
#define WAIT4			{WAIT2;WAIT2}
#define WAIT6			{WAIT4;WAIT2}
#define WAIT			WAIT2

// macro to suppress unused variable
#define UNUSED(x) (void)(x)

// LUFA CDC Class driver interface configuration
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

// standard file stream
static FILE USBSerialStream;

// board id and compile time statistics
static const char board_id[17] = {'G','B','C','R','-','A','V','R','-','V','2','.','0','.','0','\0'};
static const char cdate[17] = __DATE__;
static const char ctime[17] = __TIME__;

// command storage
char instruction[9];    // stores single 8-byte instruction
uint8_t inptr = 0;      // instruction pointer

// forward declaration
void write_board_id(void);
void compile_time();
void parse_instructions(void);
void read_header(void);
void set_upper_address(uint8_t);
void set_lower_address(uint8_t);
void read_sector(uint16_t addr);
void write_byte_at_address(uint16_t addr, uint8_t val);
void set_ram_enable(bool enable);
void write_bytes_ram(uint16_t addr, uint16_t sz);

// flashable cartridges
void sst39sf0x0_get_device_id(void);
uint16_t sst39sf0x0_pollbyte(uint16_t addr);
void sst39sf0x0_erase_sector(uint16_t erase_sector);
void sst39sf0x0_write_block(uint16_t);

int main(void) {
	SetupHardware();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	GlobalInterruptEnable();

	for (;;)
	{
		/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
		if(CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface) > 0) {
			char c = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
			
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

		// handle USB Tasks
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
}

/*
 * @brief Put pins in their default configuration
 */
void reset_pins() {
	READ_HIGH;
	WRITE_HIGH;
	SRAM_HIGH;
	RESET_HIGH;
	CPL_LOW;
	CPH_LOW;
	AUDIO_HIGH; // set audio high as it is connected to ~WE signal
	PINS_INPUT;
}

void SetupHardware(void) {
	DDRB = 0xFF;	  // output
	DDRD = 0x00;	  // input
	PINS_INPUT;
	DDRE |= (1 << 6); // output on SRAM
	DDRF = 0xFF;	  // output
	
	reset_pins();
	
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	USB_Init();
}

void EVENT_USB_Device_Connect(void) {
	// do nothing
}

void EVENT_USB_Device_Disconnect(void) {
	// do nothing
}

void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

void EVENT_USB_Device_ControlRequest(void) {
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo) {
	bool HostReady = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != 0;
	UNUSED(HostReady);
}

void echo_command(void) {
	for(uint8_t i=0; i<8; i++) {
		CDC_Device_SendByte(&VirtualSerial_CDC_Interface, instruction[i]);
	}
	CDC_Device_Flush(&VirtualSerial_CDC_Interface);
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
void parse_instructions(void) {
	echo_command();
	
	if(check_command(instruction, "READINFO", 0, 8)) {
		write_board_id();
		return;
	}  else if(check_command(instruction, "COMPTIME", 0, 8)) {
		compile_time();
		return;
	} else if(check_command(instruction, "READHDR0", 0, 8)) {
		read_header();
		return;
	} else if(check_command(instruction, "DEVIDSST", 0, 8)) {
		sst39sf0x0_get_device_id();
		return;
	} else if(check_command(instruction, "RAMON000", 0, 8)) {
		set_ram_enable(true);
		return;
	} else if(check_command(instruction, "RAMOFF00", 0, 8)) {
		set_ram_enable(false);
		return;
	} else if(check_command(instruction, "RMWR2k00", 0, 8)) {
		write_bytes_ram(0xA000, 2048);
		return;
	} else if(check_command(instruction, "RMWR4kA0", 0, 8)) {
		write_bytes_ram(0xA000, 4096);
		return;
	} else if(check_command(instruction, "RMWR4kB0", 0, 8)) {
		write_bytes_ram(0xB000, 4096);
		return;
	} else if(check_command(instruction, "RDBK", 0, 4)) {
		read_sector(get_uint16(instruction, 4));
		return;
	} else if(check_command(instruction, "WRST", 0, 4)) {
		clock_prescale_set(clock_div_2);
		sst39sf0x0_write_block(get_uint16(instruction, 4));
		clock_prescale_set(clock_div_1);
		return;
	} else if(check_command(instruction, "ESST", 0, 4)) {
		sst39sf0x0_erase_sector(get_uint16(instruction, 4));
		return;
	} else if(check_command(instruction, "WR", 0, 2)) {
		write_byte_at_address(get_uint16(instruction, 2), get_uint8(instruction, 6));
		return;
	}
}

/*
 * @brief Write board ID to serial
 */
void write_board_id(void) {
	for(uint8_t i=0; i<16; i++) {
		CDC_Device_SendByte(&VirtualSerial_CDC_Interface, board_id[i]);
	}
}

/*
 * @brief Yield compile time
 */
void compile_time() {
	for(uint8_t i=0; i<16; i++) {
		CDC_Device_SendByte(&VirtualSerial_CDC_Interface, cdate[i]);
	}
	for(uint8_t i=0; i<16; i++) {
		CDC_Device_SendByte(&VirtualSerial_CDC_Interface, ctime[i]);
	}
}

/*
 * @brief Set upper eight bits of the address
 * @param address
 */
void set_upper_address(uint8_t addr) {
	PINS_OUTPUT;
	PORTD = addr;
	CPH_TOGGLE;
	PINS_INPUT;
}

/*
 * @brief Set lower eight bits of the address
 * @param address
 */
void set_lower_address(uint8_t addr) {
	PINS_OUTPUT;
	PORTD = addr;
	CPL_TOGGLE;
	PINS_INPUT;
}

/*
 * @brief Set address
 * @param address
 */
void set_address(uint16_t addr) {
	set_upper_address(addr >> 8);
	set_lower_address(addr & 0xFF);
}

/*
 * @brief Read cartridge header
 *
 * Read the first 0x150 bytes of the cartridge
 */
void read_header() {
		
	for(uint16_t i=0; i<0x150; i++) {
		set_address(i);
		READ_LOW;	// note that these two NOPs are absolutely necessary to give the 32u4 enough time to sample the ROM
		asm volatile("nop");
		asm volatile("nop"); 
		CDC_Device_SendByte(&VirtualSerial_CDC_Interface, (uint8_t)PIND);
		READ_HIGH;
	}
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
		
		uint8_t i = 0;
		do {
			set_lower_address(i);
			READ_LOW;	// note that these two NOPs are absolutely necessary to give the 32u4 enough time to sample the ROM
			asm volatile("nop");
			asm volatile("nop");
			CDC_Device_SendByte(&VirtualSerial_CDC_Interface, (uint8_t)PIND);
			READ_HIGH;
		} while(i++ != 255);
		
		CDC_Device_Flush(&VirtualSerial_CDC_Interface);
	}
}

/*
 * @brief Enable or disable RAM banks
 * @param Whether to enable or disable ram
 */
void set_ram_enable(bool enable) {
	if(enable) {
		write_byte_at_address(0x0000, 0x0A);
		SRAM_LOW;
	} else {
		write_byte_at_address(0x0000, 0x00);
		SRAM_HIGH;
	}
}

/*
 * @brief Write bytes to RAM
 * @param number of bytes to write (typically 2kb or 4kb)
 *
 * Write sz bytes to RAM memory
 */
void write_bytes_ram(uint16_t addr, uint16_t sz) {		
	uint8_t nrblocks = sz / 0x100;
	for(uint8_t j=0; j<nrblocks; j++) {
		set_upper_address((uint8_t)(addr >> 8) + j);
		uint8_t i = 0;
		do {
			set_lower_address(i);
			PINS_OUTPUT;
			
			// keep on looping until a byte is read
			while(true) {
				if(CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface) > 0) {
					// once byte is read, write it to RAM
					PORTD = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
					WRITE_LOW;
					asm volatile("nop");
					asm volatile("nop");
					asm volatile("nop");
					asm volatile("nop");
					WRITE_HIGH;
					break;
				}
			}
		} while(i++ != 255);
	}
	
	// set PORTD back to default input state
	PINS_INPUT;
}

/*
 * @brief Write a single byte at address
 * @param address to write byte
 * @param value to write
 *
 * This function is mainly used for bank switching, for
 * writing data to RAM or to flash, single-byte write
 * operations take too much time.
 */
void write_byte_at_address(uint16_t addr, uint8_t val) {
	set_address(addr);
	PINS_OUTPUT;

	PORTD = val;
	WRITE_PULSE; // pulse write signal
	
	PINS_INPUT;
}

/*
 * @brief Write single command word to SST39sf0x0 chip
 * @param address to write byte at
 * @param byte to write
 */
void sst39sf0x0_write_command(uint16_t addr, uint8_t val) {
	set_address(addr & (~(1 << 15)));	// set address and set low
	
	PINS_OUTPUT;						// write out
	PORTD = val;
	AUDIO_LOW;							// write address
	WAIT;
	AUDIO_HIGH;							// latch data
	set_address(addr | (1 << 15));		// set CE high
}

/*
 * @brief Get device ID from SST39SF0x0 chip
 */
void sst39sf0x0_get_device_id() {
	sst39sf0x0_write_command(0x5555, 0xAA);
	sst39sf0x0_write_command(0x2AAA, 0x55);
	sst39sf0x0_write_command(0x5555, 0x90);
	
	PINS_INPUT;
	set_address(0x0000);
	READ_LOW;
	WAIT;
	uint8_t id1 = PIND;
	READ_HIGH;
	
	set_lower_address(0x01);
	READ_LOW;
	WAIT;
	uint8_t id2 = PIND;
	READ_HIGH;
	
	sst39sf0x0_write_command(0x5555, 0xAA);
	sst39sf0x0_write_command(0x2AAA, 0x55);
	sst39sf0x0_write_command(0x5555, 0xF0);
	
	CDC_Device_SendByte(&VirtualSerial_CDC_Interface, id1);
	CDC_Device_SendByte(&VirtualSerial_CDC_Interface, id2);
}

/*
 * @brief Erase sector
 */
void sst39sf0x0_erase_sector(uint16_t erase_sector) {
	// instructions to erase sector
	sst39sf0x0_write_command(0x5555, 0xAA);
	sst39sf0x0_write_command(0x2AAA, 0x55);
	sst39sf0x0_write_command(0x5555, 0x80);
	sst39sf0x0_write_command(0x5555, 0xAA);
	sst39sf0x0_write_command(0x2AAA, 0x55);
	sst39sf0x0_write_command(erase_sector, 0x30);

	// count number of waiting cycles
	uint16_t cnts = sst39sf0x0_pollbyte(erase_sector);

	// return number of waiting cycles
	CDC_Device_SendByte(&VirtualSerial_CDC_Interface, cnts >> 8);
	CDC_Device_SendByte(&VirtualSerial_CDC_Interface, cnts & 0xFF);
	CDC_Device_Flush(&VirtualSerial_CDC_Interface);
}

uint16_t sst39sf0x0_pollbyte(uint16_t addr) {
	// check if DQ7 equals true data (1), else, wait until done.
	PINS_INPUT;
	uint8_t cnts = 0;     // keep track of number of polling attempts
	uint16_t pollbyte = 0;
	while((pollbyte >> 7) != 1 && cnts < 0x1000) {
		set_address(addr);
		READ_LOW;
		WAIT;
		WAIT;
		pollbyte = PIND;
		READ_HIGH;
		cnts++;
	}
	
	return cnts;
}

/*
 * @brief Write block of 256 bytes
 */
void sst39sf0x0_write_block(uint16_t start_addr) {
	// write new data
	uint16_t addr = start_addr;
	uint16_t bitsread = 0;
	
	while(bitsread < 256) {
		if(CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface) > 0) {
						
			uint8_t c = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
			sst39sf0x0_write_command(0x5555, 0xAA);
			sst39sf0x0_write_command(0x2AAA, 0x55);
			sst39sf0x0_write_command(0x5555, 0xA0);
			sst39sf0x0_write_command(addr, c);
				
			addr++;
			bitsread++;
			
			// since SRAM is not connected, we can use the LED to give some user feedback
			if(bitsread % 64 > 32) {
				SRAM_LOW;
			} else {
				SRAM_HIGH;
			}
		}
	}
	
	// set high at end of function
	reset_pins();
}
