## Makefile.am

include $(top_srcdir)/config/Make-rules
INCLUDES = -I$(top_srcdir)/src/tools/ \
		-I$(top_srcdir)/src/gene/ \
		-I$(top_srcdir)/src/folder/ \
		-I$(top_srcdir)/src/evolver/

CLEANFILES =  runtest.cc

bin_PROGRAMS = perf-test perf-test-decoy
perf_test_LDADD = $(top_builddir)/src/evolver/libevolver.a \
		$(top_builddir)/src/folder/libfolder.a \
		$(top_builddir)/src/gene/libgene.a \
	$(top_builddir)/src/tools/libtools.a
perf_test_SOURCES = perf-test.cc
perf_test_decoy_LDADD = $(top_builddir)/src/evolver/libevolver.a \
		$(top_builddir)/src/folder/libfolder.a \
		$(top_builddir)/src/gene/libgene.a \
	$(top_builddir)/src/tools/libtools.a
perf_test_decoy_SOURCES = perf-test-decoy.cc

## Cutee unit testing directives
# your test files here
test_files = t.random.h t.codon.h t.protein.h t.genetic-code.h t.folder.h t.population.h t.fitness-evaluator.h t.tools.h t.translator.h t.mutator.h
# list their autogenerated counterparts in CLEANFILES
CLEANFILES += t.random.cutee.cc t.protein.cutee.cc t.genetic-code.cutee.cc t.codon.cutee.cc \
	t.folder.cutee.cc t.population.cutee.cc t.tools.cutee.cc t.fitness-evaluator.cutee.cc \
	t.translator.cutee.cc t.mutator.cutee.cc

noinst_PROGRAMS = cutee runtest

cutee_SOURCES = cutee.cc cutee.h

runtest_LDADD = $(top_builddir)/src/evolver/libevolver.a \
		$(top_builddir)/src/folder/libfolder.a \
		$(top_builddir)/src/gene/libgene.a \
		$(top_builddir)/src/tools/libtools.a

autocutee.mk: cutee Makefile.am $(test_files) $(runtest_LDADD)
	$(CUTEE) -k -o autocutee.mk $(test_files)

include autocutee.mk
