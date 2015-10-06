
CFPLAGS=-Wall -O3 -fPIC -Werror
CC=gcc
PLUGINDIR=/usr/lib/ladspa

all: abc.so pseudoquadro.so pseudoquadroEC.so abcEC.so

clean: 
	rm -f *.o *.so

pseudoquadro.so: pseudoquadro.c
	$(CC) $(CFPLAGS) -o pseudoquadro.o -c pseudoquadro.c
	$(LD) -o pseudoquadro.so pseudoquadro.o -shared

abc.so: abc.c
	$(CC) $(CFPLAGS) -o abc.o -c abc.c
	$(LD) -o abc.so abc.o -shared

pseudoquadroEC.so: pseudoquadroEC.c
	$(CC) $(CFPLAGS) -o pseudoquadroEC.o -c pseudoquadroEC.c
	$(LD) -o pseudoquadroEC.so pseudoquadroEC.o -shared

abcEC.so: abcEC.c
	$(CC) $(CFPLAGS) -o abcEC.o -c abcEC.c
	$(LD) -o abcEC.so abcEC.o -shared

install: all
	(if [ -d $(PLUGINDIR) ]; then cp pseudoquadro.so abc.so pseudoquadroEC.so abcEC.so $(PLUGINDIR); else echo No plugins directory; false;fi)

uninstall: deinstall

deinstall:
	rm -f $(PLUGINDIR)/pseudoquadro.so  $(PLUGINDIR)/abc.so $(PLUGINDIR)/pseudoquadroEC.so $(PLUGINDIR)/abcEC.so

