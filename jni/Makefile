CC=gcc
INCLUDE=
LIBS=-L.
SOURCES=jpegtoavi.c avifmt.h byteswap.h llist.h llist.c
DISTFILES=$(SOURCES) jpegtoavi.1 README COPYING Makefile CHANGELOG
INSTALL_PREFIX=/usr/local
AMTAR=tar
PACKAGE=jpegtoavi
PACKAGE_VERSION_MAJ=1
PACKAGE_VERSION_MIN=5
PACKAGE_VERSION=$(PACKAGE_VERSION_MAJ).$(PACKAGE_VERSION_MIN)
VERBOSE=0
CCFLAGS=$(INCLUDE) -O3 -Wall -ansi -g -DLINUX -D__UNIX__ \
	-DVERSION_MIN=$(PACKAGE_VERSION_MIN) \
	-DVERSION_MAJ=$(PACKAGE_VERSION_MAJ) \
	-DVERBOSE=$(VERBOSE)
STRIP=strip

distdir = $(PACKAGE)-$(PACKAGE_VERSION)
GZIP_ENV = --best

am__remove_distdir = \
  { test ! -d $(distdir) \
    || { find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr $(distdir); }; }

all: jpegtoavi

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

jpegtoavi: jpegtoavi.o llist.o
	$(CC) $(CCFLAGS) jpegtoavi.o llist.o $(LIBS) -o $@

distdir: $(DISTFILES)
	$(am__remove_distdir)
	mkdir $(distdir)
	cp -a $(DISTFILES) $(distdir)

dist-gzip: distdir
	$(AMTAR) chof - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

clean: 
	rm -f *.o

install : jpegtoavi jpegtoavi.1
	strip jpegtoavi
	cp jpegtoavi $(INSTALL_PREFIX)/bin
	cp jpegtoavi.1 $(INSTALL_PREFIX)/man/man1

depend:
	-$(CC) $(CCFLAGS) -MM $(SOURCES) > Makefile.dependencies
	gawk '/^# DO NOT DELETE DEPENDENCIES/{exit} /.*/{print}' Makefile > Makefile.strip
	echo '# DO NOT DELETE DEPENDENCIES' > Makefile.banner
	mv Makefile Makefile.bak
	cat Makefile.strip Makefile.banner Makefile.dependencies > Makefile
	rm -f Makefile.strip Makefile.dependencies Makefile.banner


# DO NOT DELETE DEPENDENCIES
jpegtoavi.o: jpegtoavi.c avifmt.h byteswap.h
