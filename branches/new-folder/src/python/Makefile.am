## Makefile.am

include $(top_srcdir)/config/Make-rules

all: $(top_builddir)/src/folder/libfolder.a
	python setup.py build

install: $(top_builddir)/src/folder/libfolder.a
	python setup.py install

clean:
	rm -rf build

maintainer-clean: clean


