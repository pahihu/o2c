# Note: The default case for the compiler is to generate code which does not 
#       use Boehm's garbage collector.  If you want to use the garbage 
#       collector, please edit the variables `defCodeForGC' and `gcLibPath'
#       in "compiler/OMachine.Mod" accordingly.

CC = gcc-mp-5 -m32
# where to install the binaries?
INSTALL_DIR = /opt/o2c
BINDIR = $(INSTALL_DIR)/bin
MANDIR = $(INSTALL_DIR)/share/man



# you shouldn't have to change anything below

# Note: These flags are only used to build the first (of three) compilers
CFLAGS = -O2 -DDISABLE_RTC -iquote obj -iquote system
# LDFLAGS = -lm -Wl,-s
LDFLAGS = -lm

COMMANDS = o2c o2b o2whereis o2ef
COMPILER = OBrowse ODepend OEParse OGenGCC OMachine OMakeGCC OParse OScan OSym OTable $(COMMANDS)


all: o2c_stage2
	touch all

o2c_stage0:
	if [ ! -d sym ]; then \
		mkdir sym; \
	fi
	if [ -f obj.distrib ]; then \
		-rm -rf obj; mv obj.distrib obj; rm -f obj/*.[o]; \
	fi
	-rm -f o2c_stage0 o2c_stage1 o2c_stage2 o2c all
# check the hardcoded type sizes against the C types
	$(CC) -o sizes system/sizes.c
	./sizes
# build compiler executable from the distributed C sources
	$(MAKE) -f makefile_o2c CFLAGS='$(CFLAGS)' LDFLAGS='$(LDFLAGS)' build
	mv obj obj.distrib
	mkdir obj
# build utilities, but avoid recompilation of C files
	-./o2c -MOR --redir system/o2c.red.template o2c X
	cp obj.distrib/* obj
	touch obj/*.o
	./o2c -MORsv --redir system/o2c.red.template o2b
	./o2c -MORsv --redir system/o2c.red.template o2ef
	./o2c -MORsv --redir system/o2c.red.template o2whereis
	mv o2c o2c_stage0

o2c_stage1: o2c_stage0
	-rm -f o2c_stage1 o2c_stage2 o2c all sym/* obj/*
# build second compiler from distributed vanilla sources; if the sources were
# modified for gc support, the new compiler will generate code for gc but 
# won't use gc itself
	./o2c_stage0 -MRv --redir system/o2c.red.template o2c
	mv o2c o2c_stage1

o2c_stage2: o2c_stage1
	-rm -f o2c_stage2 o2c all sym/* obj/*
# build third compiler; if the sources were modified for gc support, the new 
# compiler will use the gc package and generate code for gc
	./o2c_stage1 -MORsv --redir system/o2c.red.template o2c	
# build library modules, but keep existing object files
	-./o2c -MORv --redir system/o2c.red.template UpdateLib X
	touch obj/*.o
	./o2c -MORv --redir system/o2c.red.template UpdateLib
	mv o2c o2c_stage2

install.bin: all
	if [ ! -f $(HOME)/.o2c.red ]; then \
		cat system/o2c.red.install | sed "s+%INSTALL_DIR%+$(INSTALL_DIR)+" > $(HOME)/.o2c.red; \
	fi
	chmod -R ugo+rX *
	-rm -f o2c
	ln o2c_stage2 o2c
	if [ ! -d $(BINDIR) ]; then \
		mkdir -p $(BINDIR); \
	fi
	for i in $(COMMANDS) ; do \
		cp $$i $(BINDIR)/$$i ; \
		chmod 755 $(BINDIR)/$$i ; \
	done
	for d in compiler lib obj sym system ; do \
		cp -rp $$d $(INSTALL_DIR) ; \
	done

install.docs:
	if [ ! -d $(INSTALL_DIR)/docs ]; then \
		mkdir -p $(INSTALL_DIR)/docs; \
	fi
	cp docs/*.ps $(INSTALL_DIR)/docs
	for d in CHANGES ChangeLog COPYING README KNOWN_PROBLEMS ; do \
		cp $$d $(INSTALL_DIR) ; \
	done

install.man:
	if [ ! -d $(MANDIR)/man1 ]; then \
		mkdir -p $(MANDIR)/man1; \
	fi
	for i in $(COMMANDS) ; do \
		cp docs/$$i.1 $(MANDIR)/man1/$$i.1 ; \
		chmod 644 $(MANDIR)/man1/$$i.1 ; \
	done

install: all
	make install.bin
	make install.man
	make install.docs

clean:
	-rm -f makefile_o2c Makefile sizes
	-rm -f o2c_stage0 o2c_stage1 o2c_stage2 o2c all UpdateLib
	-rm -rf obj.distrib
	-for i in $(COMMANDS) UpdateLib ; do rm -f $$i obj/_$$i.[co] ; done
	-for i in $(COMPILER) UpdateLib ; do rm -f obj/$$i.[hco] sym/$$i.OSym ; done

restore:
	-rm -f sizes
	-if [ obj.distrib ]; then rm -rf obj; mv obj.distrib obj; fi
	-rm -f o2c_stage0 o2c_stage1 o2c_stage2 o2c all UpdateLib 
	-for i in $(COMMANDS) UpdateLib ; do rm -f $$i obj/_$$i.[o] ; done
	-for i in $(COMPILER) UpdateLib ; do rm -f obj/$$i.[o] sym/$$i.OSym ; done
