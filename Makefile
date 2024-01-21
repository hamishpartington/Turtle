CC=gcc
COMMON= -Wall -Wextra -Wfloat-equal -Wpedantic -Wvla -std=c99 #-Werror
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O3
SDL= `pkg-config sdl2 --cflags` `pkg-config sdl2 --libs`

all: parse_s interp_s parse interp extension parse_v interp_v

parse_s: Parse/parse.h Parse/parse.c
	$(CC) Parse/parse.c -o parse_s -I./Parse $(SANITIZE) -lm

parse: Parse/parse.h Parse/parse.c
	$(CC) Parse/parse.c -o parse -I./Parse $(PRODUCTION) -lm

parse_v: Parse/parse.h Parse/parse.c
	$(CC) Parse/parse.c -o parse_v -I./Parse $(VALGRIND) -lm

interp_s: Interp/interp.h Interp/interp.c neillsimplescreen.c neillsimplescreen.h Interp/Stack/stack.c Interp/Stack/stack.h
	$(CC) neillsimplescreen.c Interp/interp.c Interp/Stack/stack.c -o interp_s -I./Interp $(SANITIZE) -lm

interp_v: Interp/interp.h Interp/interp.c neillsimplescreen.c neillsimplescreen.h Interp/Stack/stack.c Interp/Stack/stack.h
	$(CC) neillsimplescreen.c Interp/interp.c Interp/Stack/stack.c -o interp_v -I./Interp $(VALGRIND) -lm

interp: Interp/interp.h Interp/interp.c neillsimplescreen.c neillsimplescreen.h Interp/Stack/stack.c Interp/Stack/stack.h
	$(CC) neillsimplescreen.c Interp/interp.c Interp/Stack/stack.c -o interp -I./Interp $(PRODUCTION) -lm

extension: Extension/extension.h Extension/extension.c
	$(CC) Extension/extension.c -o extension -I./Extension $(PRODUCTION) $(SDL) -lm

# Crete a .zip file using ALL files in this directory
zip: Parse Interp Extension
	zip -r turtle.zip Extension Interp Neill_Results Parse Results Testing TTLs grammar.txt neillsimplescreen.c neillsimplescreen.h Makefile 

run: all
	./parse TTLs/empty.ttl
	./parse TTLs/forward.ttl
	./parse TTLs/ok_parse_fail_interp.ttl
	./parse TTLs/set1.ttl
	./parse TTLs/donothing.ttl
	./parse TTLs/set2.ttl
	./parse TTLs/turn.ttl
	./parse TTLs/spiral.ttl
	./parse TTLs/octagon1.ttl
	./parse TTLs/octagon2.ttl
	./parse TTLs/tunnel.ttl
	./parse TTLs/labyrinth.ttl
	./parse TTLs/hypno.ttl
	./parse TTLs/5x5.ttl
	./parse TTLs/downarrow.ttl
	./interp TTLs/empty.ttl out_empty.txt
	./interp TTLs/forward.ttl out_forward.txt
	./interp TTLs/set1.ttl out_set1.txt
	./interp TTLs/donothing.ttl out_donothing.txt
	./interp TTLs/set2.ttl out_set2.txt
	./interp TTLs/turn.ttl out_turn.txt
	./interp TTLs/spiral.ttl out_spiral.txt
	./interp TTLs/octagon1.ttl out_octagon1.txt
	./interp TTLs/octagon2.ttl out_octagon2.txt
	./interp TTLs/tunnel.ttl out_tunnel.txt
	./interp TTLs/labyrinth.ttl out_labyrinth.txt
	./interp TTLs/hypno.ttl out_hypno.txt
	./interp TTLs/5x5.ttl out_5x5.txt
	./interp TTLs/downarrow.ttl out_downarrow.txt
	./interp TTLs/fail_parse_ok_interp.ttl out_fail_parse_ok_interp.txt
	./interp TTLs/empty.ttl out_empty.ps
	./interp TTLs/forward.ttl out_forward.ps
	./interp TTLs/set1.ttl out_set1.ps
	./interp TTLs/donothing.ttl out_donothing.ps
	./interp TTLs/fail_parse_ok_interp.ttl out_fail_parse_ok_interp.ps
	./interp TTLs/set2.ttl out_set2.ps
	./interp TTLs/turn.ttl out_turn.ps
	./interp TTLs/spiral.ttl out_spiral.ps
	./interp TTLs/octagon1.ttl out_octagon1.ps
	./interp TTLs/octagon2.ttl out_octagon2.ps
	./interp TTLs/tunnel.ttl out_tunnel.ps
	./interp TTLs/labyrinth.ttl out_labyrinth.ps
	./interp TTLs/hypno.ttl out_hypno.ps
	./interp TTLs/5x5.ttl out_5x5.ps
	./interp TTLs/downarrow.ttl out_downarrow.ps

clean:
	rm -f parse_s interp_s parse interp parse_v interp_v Results/out_*.txt Results/out_*.ps Results/out_*.pdf
