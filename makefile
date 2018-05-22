# change application name here (executable output name)
TARGET=netHole-gtk3
 
# compiler
CC=gcc
#Debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall
 
PTHREAD=-pthread
DNET=-ldnet
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN)
# $(PTHREAD) $(GTKLIB)-pipe
 
AM_CPPFLAGS=-I./inc -I/usr/local/include -I/usr/include/pcap
 
# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) $(DNET) -lX11 -export-dynamic 

SOURCES=src/main.c src/netHole.c src/temp_utils.c

OBJS=$(SOURCES:.c=.o)
 
#all: 
#	$(LD) $(TARGET) $(LDFLAGS) $(AM_CPPFLAGS) -o
all: $(TARGET)

%.o: %.c
	$(CC) -c $^ $(CCFLAGS) $(LDFLAGS) $(AM_CPPFLAGS) -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(CCFLAGS) $(AM_CPPFLAGS) -o $(TARGET)
#main.o: 
#	$(CC) -c $(CCFLAGS) $(SOURCES) $(GTKLIB) -o main.o
    
clean:
	rm -f *.o $(TARGET)