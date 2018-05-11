
CC=gcc
RM=rm -f

CXXFLAGS=-Wall -O3

OBJS=jetson_tx2_power.o jetson_tx2_power_main.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)
	$(CC) $(LDFLAGS) -o pwr $(OBJS) $(LDLIBS)

clean:
	$(RM) *.o pwr
