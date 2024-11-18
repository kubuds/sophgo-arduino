ARCH_AARCH64 = aarch64

ifeq ($(ARCH_AARCH64), $(ARCH))
	CROSS_COMPILE = aarch64-linux-gnu-
	LDFLAGS := -L $(shell pwd)/lib/aarch64 -lev -lm -lz -lpthread
else
	CROSS_COMPILE = riscv64-linux-gnu-
	LDFLAGS := -L $(shell pwd)/lib/riscv64 -lev -lm -lz -lpthread
endif


TARGET := burnd
CFLAGS = -Wall --static -O

CFLAGS  += -I$(shell pwd)/include 

CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

SRC = $(wildcard src/*.c)
OBJS:= $(patsubst %.c, %.o, $(SRC))

all :$(OBJS)
	$(CC) $(OBJS) --static -o $(TARGET) $(LDFLAGS)
	$(STRIP) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean:
	rm src/*.o burnd
