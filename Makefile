CFLAG=-lncurses
OUTPUT=bin/meteormayhem
SOURCE=src/*.c

${OUTPUT}: ${SOURCE}
	if [ ! -d bin ]; then mkdir bin; fi;
	gcc -o ${OUTPUT} ${SOURCE} ${CFLAG}

clean:
	rm -rf bin/

