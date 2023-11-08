CC:=gcc
CFLAGS:=-pipe -std=gnu17 -Wall -Wextra -Werror -Og -ggdb

OUT:=lg-crc32

.PHONY: all
all: $(OUT)

.PHONY: clean
clean:
	rm -f -- $(OUT)
