#include <stddef.h>
#include "c65.h"

#define PN 0x80
#define PV 0x40

#define PB 16
#define PD 8
#define PI 4
#define PZ 2
#define PC 1

#define setnz(x) c->p = (c->p & 0x7D) | ((!x) << 1) | (x & 0x7F)
#define pull_noinc() (c->r(c, 0x100|(c->s)))
#define pull() (c->r(c, 0x100|(++c->s)))
#define push(x) c->w(c, 0x100|(c->s--), x)
#define op(x) static void x(c65* c)
// yes these end with two braces. dont worry about it
#define swop(x) static void x(c65* c) { switch(c->cycle)

swop(i_brk) {
	case 0:
		c->r(c, c->ip);
		break;
	case 1:
		push(c->ip >> 8);
		break;
	case 2:
		push(c->ip);
		break;
	case 3:
		push(c->p | PB); // TODO is B set in-register too?
		break;
	case 4:
		c->ip = c->r(c, 0xFFFE);
		break;
	case 5:
		c->ip |= c->r(c, 0xFFFF) << 8;
		c->end++;
		break;
}}

swop(s_plpa) {
	case 0:
		c->r(c, c->ip);
		break;
	case 1:
		pull();
		break;
	case 2:
		uint8_t s = pull_noinc();
		if(c->op == 0x20) c->p = s | 0x20; // bit 5 always set
		else {
			setnz(s);
			c->a = s;
		}
		c->end++;
		break;
}}

op(s_phpa) {
	// https://www.nesdev.org/wiki/Status_flags#The_B_flag
	switch(c->cycle) {
		case 0:
			c->r(c, c->ip);
			break;
		case 1:
			push(((c->op & 0x40) ? c->a : c->p) | 0x10); // php always pushes brk flag as set
			c->end++;
			break;
	}
}

op(i_sef) {
	static uint8_t t[4] = {0, 2, 6, 3};
	uint8_t s = t[c->op >> 6];
	c->p &= ~(1 << s) | (((c->op >> 5) & 1) << s); // set or clear flag depending on value of bit 5
	c->r(c, c->ip); // TODO best guess
	c->end++;
}

swop(a_jmp) {
	case 0:
		c->b = c->r(c, c->ip++);
		break;
	case 1:
		c->ip = (c->r(c, c->ip) << 8) | c->b;
		c->end++;
		break;
}}

swop(r_bcc) { // TODO verify implementation
	case 0:
		c->b = c->r(c, c->ip++);
		static char m[4] = {7, 6, 0, 9};
		char d = m[c->op >> 6];
		if(
			(
				(
					c->p >> (d & 7)
				) & 1
			) ^ (d >> 3)
		) c->end++; // branch not taken
		break;
	case 1:
		c->ip++; // TODO correct?
		c->r(c, ((c->ip + c->b) & 0xFF) | (c->ip & 0xFF00));
		if(!((((c->ip & 0xFF) + ((int8_t)c->b)) >> 8) & 1)) c->end++; // no page crossing
		c->ip += ((int8_t)c->b);
		break;
	case 2:
		c->r(c, c->ip);
		break;
}}

op(i_trans) {
	switch(c->op) {
		case 0x8A:
			c->a = c->x;
			setnz(c->a);
			break;
		case 0x98:
			c->a = c->y;
			setnz(c->a);
			break;
		case 0x9A:
			c->s = c->x;
			break;
		case 0xA8:
			c->y = c->a;
			setnz(c->y);
			break;
		case 0xAA:
			c->x = c->a;
			setnz(c->x);
			break;
		case 0xBA:
			c->x = c->s;
			setnz(c->x);
			break;
	}
	c->r(c, c->ip); // TODO best guess
	c->end++;
}

op(i_dec) {
	switch(c->op) {
		case 0x88:
			c->y--;
			setnz(c->y);
			break;
		case 0xC8:
			c->y++;
			setnz(c->y);
			break;
		case 0xCA:
			c->x--;
			setnz(c->x);
			break;
		case 0xE8:
			c->x++;
			setnz(c->x);
			break;
	}
}

static void(*t[256])(c65*) = {
	i_brk,
	NULL, // 01
	NULL, // 02
	NULL, // 03
	NULL, // 04
	NULL, // 05
	NULL, // 06
	NULL, // 07
	s_phpa, // 08
	NULL, // 09
	NULL, // 0A
	NULL, // 0B
	NULL, // 0C
	NULL, // 0D
	NULL, // 0E
	NULL, // 0F
	r_bcc, // 10
	NULL, // 11
	NULL, // 12
	NULL, // 13
	NULL, // 14
	NULL, // 15
	NULL, // 16
	NULL, // 17
	i_sef, // 18
	NULL, // 19
	NULL, // 1A
	NULL, // 1B
	NULL, // 1C
	NULL, // 1D
	NULL, // 1E
	NULL, // 1F
	NULL, // 20
	NULL, // 21
	NULL, // 22
	NULL, // 23
	NULL, // 24
	NULL, // 25
	NULL, // 26
	NULL, // 27
	s_plpa, // 28
	NULL, // 29
	NULL, // 2A
	NULL, // 2B
	NULL, // 2C
	NULL, // 2D
	NULL, // 2E
	NULL, // 2F
	r_bcc, // 30
	NULL, // 31
	NULL, // 32
	NULL, // 33
	NULL, // 34
	NULL, // 35
	NULL, // 36
	NULL, // 37
	i_sef, // 38
	NULL, // 39
	NULL, // 3A
	NULL, // 3B
	NULL, // 3C
	NULL, // 3D
	NULL, // 3E
	NULL, // 3F
	NULL, // 40
	NULL, // 41
	NULL, // 42
	NULL, // 43
	NULL, // 44
	NULL, // 45
	NULL, // 46
	NULL, // 47
	s_phpa, // 48
	NULL, // 49
	NULL, // 4A
	NULL, // 4B
	a_jmp, // 4C
	NULL, // 4D
	NULL, // 4E
	NULL, // 4F
	r_bcc, // 50
	NULL, // 51
	NULL, // 52
	NULL, // 53
	NULL, // 54
	NULL, // 55
	NULL, // 56
	NULL, // 57
	i_sef, // 58
	NULL, // 59
	NULL, // 5A
	NULL, // 5B
	NULL, // 5C
	NULL, // 5D
	NULL, // 5E
	NULL, // 5F
	NULL, // 60
	NULL, // 61
	NULL, // 62
	NULL, // 63
	NULL, // 64
	NULL, // 65
	NULL, // 66
	NULL, // 67
	s_plpa, // 68
	NULL, // 69
	NULL, // 6A
	NULL, // 6B
	NULL, // 6C
	NULL, // 6D
	NULL, // 6E
	NULL, // 6F
	r_bcc, // 70
	NULL, // 71
	NULL, // 72
	NULL, // 73
	NULL, // 74
	NULL, // 75
	NULL, // 76
	NULL, // 77
	i_sef, // 78
	NULL, // 79
	NULL, // 7A
	NULL, // 7B
	NULL, // 7C
	NULL, // 7D
	NULL, // 7E
	NULL, // 7F
	NULL, // 80
	NULL, // 81
	NULL, // 82
	NULL, // 83
	NULL, // 84
	NULL, // 85
	NULL, // 86
	NULL, // 87
	i_dec, // 88
	NULL, // 89
	i_trans, // 8A
	NULL, // 8B
	NULL, // 8C
	NULL, // 8D
	NULL, // 8E
	NULL, // 8F
	r_bcc, // 90
	NULL, // 91
	NULL, // 92
	NULL, // 93
	NULL, // 94
	NULL, // 95
	NULL, // 96
	NULL, // 97
	i_trans, // 98
	NULL, // 99
	i_trans, // 9A
	NULL, // 9B
	NULL, // 9C
	NULL, // 9D
	NULL, // 9E
	NULL, // 9F
	NULL, // A0
	NULL, // A1
	NULL, // A2
	NULL, // A3
	NULL, // A4
	NULL, // A5
	NULL, // A6
	NULL, // A7
	i_trans, // A8
	NULL, // A9
	i_trans, // AA
	NULL, // AB
	NULL, // AC
	NULL, // AD
	NULL, // AE
	NULL, // AF
	r_bcc, // B0
	NULL, // B1
	NULL, // B2
	NULL, // B3
	NULL, // B4
	NULL, // B5
	NULL, // B6
	NULL, // B7
	i_sef, // B8
	NULL, // B9
	i_trans, // BA
	NULL, // BB
	NULL, // BC
	NULL, // BD
	NULL, // BE
	NULL, // BF
	NULL, // C0
	NULL, // C1
	NULL, // C2
	NULL, // C3
	NULL, // C4
	NULL, // C5
	NULL, // C6
	NULL, // C7
	i_dec, // C8
	NULL, // C9
	i_dec, // CA
	NULL, // CB
	NULL, // CC
	NULL, // CD
	NULL, // CE
	NULL, // CF
	r_bcc, // D0
	NULL, // D1
	NULL, // D2
	NULL, // D3
	NULL, // D4
	NULL, // D5
	NULL, // D6
	NULL, // D7
	i_sef, // D8
	NULL, // D9
	NULL, // DA
	NULL, // DB
	NULL, // DC
	NULL, // DD
	NULL, // DE
	NULL, // DF
	NULL, // E0
	NULL, // E1
	NULL, // E2
	NULL, // E3
	NULL, // E4
	NULL, // E5
	NULL, // E6
	NULL, // E7
	i_dec, // E8
	NULL, // E9
	NULL, // EA
	NULL, // EB
	NULL, // EC
	NULL, // ED
	NULL, // EE
	NULL, // EF
	r_bcc, // F0
	NULL, // F1
	NULL, // F2
	NULL, // F3
	NULL, // F4
	NULL, // F5
	NULL, // F6
	NULL, // F7
	i_sef, // F8
	NULL, // F9
	NULL, // FA
	NULL, // FB
	NULL, // FC
	NULL, // FD
	NULL, // FE
	NULL, // FF
};

void c65_s(c65* c) {
	if(c->end) {
		c->op = c->r(c, c->ip);	// fetch should not be part of the opcode cycle
		c->cycle = c->end = 0;	// op. impl. expected to end with ip set to next
					// instruction's address - meant for jmp mostly
	} else {
		if(t[c->op]) t[c->op](c);
		c->cycle++;
	}
}
