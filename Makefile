all: compilelibraries compilecode compileapps

#=================================================

compileapps: server client main

server: compiled/sside.o compiled/lib/encrypt.o compiled/lib/linkedlist.o 
	gcc -pthread -Wall compiled/sside.o compiled/lib/encrypt.o compiled/lib/linkedlist.o  -o server
	
client: compiled/clside.o compiled/lib/encrypt.o 
	gcc -pthread -Wall compiled/clside.o compiled/lib/encrypt.o -o client
	
main: compiled/main.o compiled/lib/encrypt.o 
	gcc -pthread -Wall compiled/main.o compiled/lib/encrypt.o -o main

#=================================================

compilecode: compiled/clside.o compiled/sside.o compiled/main.o

compiled/clside.o: source/clside.c source/clside.h
	gcc -c -pthread -Wall source/clside.c -o compiled/clside.o
	
compiled/sside.o: source/sside.c source/sside.h
	gcc -c -pthread -Wall source/sside.c -o compiled/sside.o
	
compiled/main.o: source/main.c source/main.h
	gcc -c -pthread -Wall source/main.c -o compiled/main.o

#================================================

compilelibraries: compiled/lib/encrypt.o compiled/lib/linkedlist.o 
	
compiled/lib/encrypt.o:
	gcc -c -lm -Wall source/lib/encrypt.c -o compiled/lib/encrypt.o

compiled/lib/linkedlist.o:
	gcc -c -lm -Wall source/lib/linkedlist.c -o compiled/lib/linkedlist.o

#=================================================

clean: cleancompiled cleanlibraries cleanapps

cleanapps:
	rm -f app server client

cleanlibraries:
	rm -f compiled/lib/*.o

cleancompiled:
	rm -f compiled/*.o