#include "c65.c" // dont worry about it
#include <stdio.h>

int main() {
	printf("<style>th{padding-left:5px}a{color:#bc43b8}a:visited{color:pink}body{background:#0f000f;color:#fc83f8;transform:translate(-50%,0%);left:50%;position:absolute}h2{margin:0;padding:0}</style><i><h1>c65 progress</h1></i><table style='text-align:right;border:1px solid;border-collapse:collapse'><tr>"); // dont worry about it
	int i = 0;
	for(int y = 0; y < 17; y++) {
		printf("<tr>");
		for(int x = 0; x < 17; x++) {
			if(!y) {
				if(x) printf("<th>_%X</th>", x - 1);
				else printf("<th>__</th>");
			}
			if(!x) {
				if(y) printf("<td>%X_</td>", y - 1);
			}
			uint8_t ind = (x-1) | ((y-1) << 4);
			if(y && x) printf("<td title='%02X' style='color:%s</td>", ind, t[ind] ? "green'>y" : "red'>."); // dont worry about it
			if(t[ind]) i++;
		}
		printf("</tr>");
	}
	printf("</table><i>mouse over a table entry to see the opcode number<br>if you have an <b style='color:red'>NMOS</b> 6502 and a logic probe, dm me<br><br>%i opcodes implemented (%f%%)</br>%i opcodes not (%f%%)<br><br>source code eventually...</i>", i, 25.0*i/64, 256-i, 25.0*(256-i)/64);
}

// c is a scripting language if youre stupid enough
