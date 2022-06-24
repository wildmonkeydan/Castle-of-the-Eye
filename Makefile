.PHONY: all cleansub
all:
	mkpsxiso -y ./iso.xml
cleansub:
	$(MAKE) clean
	rm -f Castle.cue Castle.bin
	
TARGET = Castle

SRCS = main.c psm.c clip.c lookat.c \

include ../common.mk 
