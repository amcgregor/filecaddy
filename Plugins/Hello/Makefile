CC	=m68k-palmos-gcc
CFLAGS	=-Wall -O2
LDFLAGS	=-nostartfiles
APPNAME	=plugin
DBNAME	=HelloPlug

all: bin.stamp $(APPNAME) $(APPNAME).prc

#$(APPNAME).prc: $(APPNAME) bin.stamp

$(APPNAME): $(APPNAME).o

$(APPNAME).o: $(APPNAME).c

bin.stamp: $(APPNAME).rcp
	pilrc $?
	touch bin.stamp

$(APPNAME).prc:
	build-prc $(APPNAME).prc "$(DBNAME)" FCad $(APPNAME) *.bin --no-check-resources --type PLUG

.PHONY: all clean veryclean install

clean:
	-rm -f *.o *.bin plugin bin.stamp

veryclean:
	-rm -f *.o *.bin *.prc plugin bin.stamp

install:
	-pilot-xfer -i $(APPNAME).prc