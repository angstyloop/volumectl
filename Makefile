# Production: just build the server. Delete any helper CLI dev tools.
#
#	   make

prod: bin/server
	make bin/server && \
	rm -f client set_volume msleep

bin/server: src/server.c
	gcc -Wall -g -o bin/server src/server.c -lasound

# Development: build other helpful command line tools, too.
#
#	   make dev

dev: bin/server bin/client bin/set_volume bin/msleep
	make prod && \
	make bin/client bin/set_volume bin/msleep bin/server

bin/client: src/client.c
	gcc -Wall -g -o bin/client src/client.c

bin/set_volume: include/set_volume.h
	cp include/set_volume.h include/set_volume.c && \
	gcc -o bin/set_volume -Dtest_main include/set_volume.c -lasound && \
	rm include/set_volume.c

bin/msleep: include/msleep.h
	cp include/msleep.h include/msleep.c && \
	gcc -o bin/msleep -Dtest_main include/msleep.c && \
	rm include/msleep.c

# Remove all compiled executable files.
clean: bin/client bin/set_volume bin/msleep
	rm -f bin/*

install:
	ln -s -f bin/server /usr/local/bin/volumectl && \
	mkdir -p /etc/systemd/system && \
	cp volumectl.service /etc/systemd/system && \
	systemctl daemon-reload && \
    	systemctl enable volumectl

