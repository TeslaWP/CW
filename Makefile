all: compilelibraries compilecode compileapps

#=================================================

compileapps: server client main

server:
	gcc -o server compiled/sside.o -g compiled/lib/encrypt.o
	
client:
	gcc -o server compiled/sside.o -g compiled/lib/encrypt.o
	
main:
	gcc -o main compiled/main.o

#=================================================

compilecode: compiled/clside.o compiled/sside.o

compiled/clside.o: source/clside.c source/clside.h
	gcc -pthread source/clside.c -o compiled/clside.o
	
compiled/sside.o: source/sside.c source/sside.h
	gcc -pthread source/sside.c -o compiled/sside.o
	
compiled/main.o: source/main.c source/main.h
	gcc -pthread source/main.c -o compiled/main.o

#================================================

compilelibraries: compiled/lib/encrypt.o
	
compiled/lib/encrypt.o:
	gcc -c -lm source/lib/encrypt.c -o compiled/lib/encrypt.o

#=================================================

clean: cleancompiled cleanlibraries cleanapps

cleanapps:
	rm -f app server client

cleanlibraries:
	rm -f compiled/lib/*.o

cleancompiled:
	rm -f compiled/*.o