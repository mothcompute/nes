#include <stdint.h>

typedef struct {
	uint16_t (*r)(void*,uint16_t);		// r/w data from memory
	void (*w)(void*,uint16_t,uint8_t);	// first arg is a c65*

	uint8_t a, x, y, s, p,			// registers

		cycle, op;			// TAke a look, y'all:
						// https://demozoo.org/productions/322616

	uint8_t b, end;				// internal registers for use in the
	uint16_t c;				// emulator. not accurate to actual
						// pla 'microcode' of the 6502

	uint16_t ip;				// instruction pointer
} c65;
