strobo : strobo.c utils.c utils.h hd77480.h
	msp430-gcc -O2 -mmcu=msp430g2452 strobo.c utils.c -o strobo
prog : strobo
	mspdebug rf2500 "prog strobo"
