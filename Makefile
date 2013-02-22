SOURCES = ./src/main.c ./src/pong.c ./src/common.h ./src/values.h
CFLAGS = $(shell allegro-config --cflags) -g -Wall
LDFLAGS = $(shell allegro-config --libs)

pong: $(SOURCES) 
	$(CC) $(SOURCES) $(OUTPUT_OPTION) $(CFLAGS) $(LDFLAGS)

clean:
	rm pong
