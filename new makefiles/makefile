# # change application name here (executable output name)
# TARGET=netHole-gtk3
 
# # compiler
# CC=gcc
# # debug
# DEBUG=-g
# # optimisation
# OPT=-O0
# # warnings
# WARN=-Wall
 
# PTHREAD=-pthread
 
# CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe
 
# GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

# SRC=netHole.c main.c
# OBJS=$(SRC:.c=.o)
 
# # linker
# LD=gcc
# LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic
 
# OBJS=    main.o
 
# # old version 
# # all: $(OBJS)
# # 	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
# all: netHole

# %.o: %.c
# 	$(CC) -c -o $(@F) $(CFLAGS) $<

# netHole:  $(OBJS)
# 	$(CC) -o $(@F) $(OBJS) 

# # main.o: src/netHole.c
# # 	$(CC) -c $(CCFLAGS) src/netHole.c $(GTKLIB) -o main.o
    
# clean:
# 	rm -f *.o $(TARGET)
# 	rm -f $(OBJS)

CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) --cflags gtk+-3.0)
LIBS = $(shell $(PKGCONFIG) --libs gtk+-3.0)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

SRC = netHole.c main.c

OBJS = $(SRC:.c=.o)

all: netHole

%.o: %.c
	$(CC) -c -o $(CFLAGS) $<

netHole: $(OBJS)
	$(CC) -o $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f netHole
