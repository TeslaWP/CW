all: compilelibraries compilecode compileapps

#=================================================

compileapps: server client main

server: compiled/sside.o compiled/lib/encrypt.o
	gcc -pthread compiled/sside.o compiled/lib/encrypt.o -o server
	
client: compiled/clside.o compiled/lib/encrypt.o
	gcc -pthread compiled/clside.o compiled/lib/encrypt.o -o client
	
main: compiled/main.o compiled/lib/encrypt.o
	gcc -pthread compiled/main.o compiled/lib/encrypt.o -o main

#=================================================

compilecode: compiled/clside.o compiled/sside.o compiled/main.o

compiled/clside.o: source/clside.c source/clside.h
	gcc -c -pthread source/clside.c -o compiled/clside.o
	
compiled/sside.o: source/sside.c source/sside.h
	gcc -c -pthread source/sside.c -o compiled/sside.o
	
compiled/main.o: source/main.c source/main.h
	gcc -c -pthread source/main.c -o compiled/main.o

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