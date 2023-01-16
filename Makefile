# Production: just build the server. Delete any helper CLI dev tools.
#
#	   make

prod: server
	make server && \
	rm -f client set_volume msleep

server: server.c
	gcc -Wall -g -o server server.c -lasound

# Development: build other helpful command line tools, too.
#
#	   make dev

dev: server client set_volume msleep
	make server client set_volume msleep

client: client.c
	gcc -Wall -g -o client client.c

set_volume: set_volume.h
	cp set_volume.h set_volume.c && \
	gcc -o set_volume -Dtest_main set_volume.c -lasound && \
	rm set_volume.c

msleep: msleep.h
	cp msleep.h msleep.c && \
	gcc -o msleep -Dtest_main msleep.c && \
	rm msleep.c

# Remove all compiled executable files.
clean: client set_volume msleep
	rm -f client set_volume msleep server
