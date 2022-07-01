.PHONY: all cleansub
all:
	mkpsxiso -y ./iso.xml
cleansub:
	$(MAKE) clean
	rm -f Castle.cue Castle.bin
	
TARGET = Castle

SRCS = main.c cd.c display.c gameflow.c input.c menu.c ui.c testSuite.c clip.c gte.c lookat.c \

include ../common.mk 
