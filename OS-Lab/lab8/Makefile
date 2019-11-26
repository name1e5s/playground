CC := gcc
CFLAGS := -lpthread -g -pg -DLEVEL1_DCACHE_LINESIZE=64

%: %.c
	$(CC) -o $@ $< $(CFLAGS)

all: 8.2.1 8.2.2 8.2.3 8.2.3.lock_free 8.2.4 8.2.4.2 8.2.5 8.2.5.2

clean:
	rm -rf 8.2.1 8.2.2 8.2.3 8.2.3.lock_free 8.2.4 8.2.4.2 8.2.5 8.2.5.2
