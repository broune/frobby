# ***** Variables

rawSources = main.cpp Action.cpp IOParameters.cpp						\
  IrreducibleDecomAction.cpp fplllIO.cpp IOHandler.cpp fourti2.cpp		\
  randomDataGenerators.cpp MonosIOHandler.cpp BigIdeal.cpp				\
  TransformAction.cpp Macaulay2IOHandler.cpp NewMonosIOHandler.cpp		\
  HelpAction.cpp stdinc.cpp DynamicFrobeniusAction.cpp					\
  dynamicFrobeniusAlgorithm.cpp GenerateIdealAction.cpp					\
  GenerateFrobeniusAction.cpp intersect.cpp FrobeniusAction.cpp			\
  Facade.cpp IOFacade.cpp DynamicFrobeniusFacade.cpp					\
  GenerateDataFacade.cpp AnalyzeAction.cpp IdealFacade.cpp				\
  Parameter.cpp ParameterGroup.cpp IntegerParameter.cpp					\
  SliceParameters.cpp BoolParameter.cpp Scanner.cpp Partition.cpp		\
  StringParameter.cpp Term.cpp TermTranslator.cpp Timer.cpp				\
  VarNames.cpp LatticeFormatAction.cpp SliceAlgorithm.cpp Ideal.cpp		\
  IntersectionAction.cpp IntersectFacade.cpp							\
  AssociatedPrimesAction.cpp MsmSlice.cpp IndependenceSplitter.cpp		\
  Projection.cpp MsmStrategy.cpp lattice.cpp LatticeFacade.cpp			\
  DecomRecorder.cpp TermGrader.cpp Fourti2IOHandler.cpp					\
  NullIOHandler.cpp Minimizer.cpp AlexanderDualAction.cpp frobby.cpp	\
  BigTermConsumer.cpp TranslatingTermConsumer.cpp HilbertAction.cpp		\
  HilbertSlice.cpp Polynomial.cpp CanonicalCoefTermConsumer.cpp			\
  HilbertStrategy.cpp Slice.cpp SliceStrategyCommon.cpp					\
  DebugStrategy.cpp OptimizeStrategy.cpp SliceFacade.cpp				\
  BigTermRecorder.cpp CoCoA4IOHandler.cpp SingularIOHandler.cpp			\
  TotalDegreeCoefTermConsumer.cpp BigPolynomial.cpp						\
  CoefBigTermRecorder.cpp PolyTransformAction.cpp VarSorter.cpp			\
  SliceEvent.cpp TermConsumer.cpp NullTermConsumer.cpp					\
  CoefTermConsumer.cpp NullCoefTermConsumer.cpp							\
  TranslatingCoefTermConsumer.cpp PolynomialFacade.cpp					\
  HilbertBasecase.cpp HilbertIndependenceConsumer.cpp					\
  SplitStrategy.cpp CanonicalTermConsumer.cpp StatisticsStrategy.cpp	\
  TestAction.cpp NameFactory.cpp error.cpp DebugAllocator.cpp			\
  FrobbyStringStream.cpp SliceStrategy.cpp PrimaryDecomAction.cpp		\
  IdealComparator.cpp IOHandlerCommon.cpp IrreducibleIdealSplitter.cpp	\
  DataType.cpp IdealConsolidator.cpp CoefBigTermConsumer.cpp			\
  PolynomialConsolidator.cpp OptimizeAction.cpp							\
  MaximalStandardAction.cpp test/Test.cpp test/TestCase.cpp				\
  test/TestQualifier.cpp test/TestRunner.cpp test/TestSuite.cpp			\
  test/TestVisitor.cpp test/macroes.cpp test/asserts.cpp				\
  LibAlexanderDualTest.cpp LibHilbertPoincareTest.cpp					\
  LibIrreducibleDecomTest.cpp IdealFactory.cpp PolynomialFactory.cpp	\
  LibMaxStdTest.cpp LibStdProgramTest.cpp LibTest.cpp					\
  OptimizeStrategyTest.cpp TermTest.cpp TermTranslatorTest.cpp			\
  TermGraderTest.cpp CountingIOHandler.cpp test/TestSorter.cpp			\
  DimensionAction.cpp LibDimensionTest.cpp SizeMaxIndepSetAlg.cpp

# This is for Mac 10.5. On other platforms this does not hurt, though
# it would be nicer to not do it then. The same thing is true of
# -L/sw/lib for ldflags.
ifndef GMP_INC_DIR
  GMP_INC_DIR="/sw/include"
endif

ifndef ldflags
  ldflags = $(LDFLAGS) -lgmpxx -lgmp -L/sw/lib
endif

ifndef CXX
  CXX      = "g++"
endif

ifndef BIN_INSTALL_DIR
  BIN_INSTALL_DIR = "/usr/local/bin/"
endif

cflags = $(CFLAGS) $(CPPFLAGS) -Wall -ansi -pedantic -I $(GMP_INC_DIR)	\
         -Wno-uninitialized -Wno-unused-parameter
program = frobby
library = libfrobby.a
benchArgs = $(FROBBYARGS)

ifndef MODE
 MODE=release
endif

MATCH=false
ifeq ($(MODE), release)
  outdir = bin/release/
  cflags += -O2
  MATCH=true
endif
ifeq ($(MODE), debug)
  outdir = bin/debug/
  cflags += -g -D DEBUG -fno-inline -Werror -Wextra -Wno-uninitialized \
            -Wno-unused-parameter
  MATCH=true
endif
ifeq ($(MODE), shared)
  outdir = bin/shared/
  cflags += -O2 -fPIC
  library = libfrobby.so
  MATCH=true
endif
ifeq ($(MODE), profile)
  outdir = bin/profile/
  cflags += -g -pg -O2 -D PROFILE
  ldflags += -pg
  MATCH=true
  benchArgs = _profile $(FROBBYARGS)
endif
ifeq ($(MODE), analysis)
  outdir = bin/analysis/
  cflags += -Wextra -fsyntax-only -O1 -Wfloat-equal -Wundef				\
  -Wno-endif-labels -Wshadow -Wlarger-than-1000 -Wpointer-arith			\
  -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wsign-compare	\
  -Waggregate-return -Wmissing-noreturn -Wmissing-format-attribute		\
  -Wno-multichar -Wno-deprecated-declarations -Wpacked					\
  -Wno-redundant-decls -Wunreachable-code -Winline						\
  -Wno-invalid-offsetof -Winvalid-pch -Wlong-long						\
  -Wdisabled-optimization -D DEBUG -Werror
  MATCH=true
endif

ifeq ($(MATCH), false)
  $(error Unknown value of MODE: "$(MODE)")
endif

sources = $(patsubst %.cpp, src/%.cpp, $(rawSources))
objs    = $(patsubst %.cpp, $(outdir)%.o, $(rawSources))

# ***** Compilation

.PHONY: all depend clean bin/$(program) test library distribution clear

all: bin/$(program) $(outdir)$(program)

# ****************** Testing
# use TESTARGS of
#  _valgrind to run under valgrind.
#  _debugAlloc to test recovery when running out of memory.
#  _full to obtain extra tests by verifying relations
#    between outputs of different actions, and generally testing
#    everything that can be tested.
# _full cannot follow the other options because it is picked up at an earlier
# point in the test system than they are. There are more options - see
# test/testScripts/testhelper for a full list.
#
# Only miniTest and bareTest support TESTARGS, and some options are not
# available unless MODE=debug.

# The correct choice to do a reasonably thorough test of an
# installation of Frobby.
test: all
	test/runTests

# Run all tests that it makes sense to run.
fullTest: all
	test/runTests _full
	test/runSplitTests _few

# Good for testing Frobby after a small change.
microTest: all
	test/runTests _few $(TESTARGS)
miniTest: all
	test/runTests $(TESTARGS)

# Runs all tests and allows full control over the arguments.
bareTest: all
	test/runTests $(TESTARGS) 
	test/runSplitTests $(TESTARGS)

# Run benchmarks to detect performance regressions. When MODE=profile,
# profile files for the benchmarked actions will be placed in bin/.
bench: all
	cd test/bench; ./runbench $(benchArgs)
benchHilbert: all
	cd test/bench; ./run_hilbert_bench $(benchArgs)
benchOptimize: all
	cd test/bench; ./run_optimize_bench $(benchArgs)
benchAlexdual: all
	cd test/bench; ./run_alexdual_bench $(benchArgs)

# Make symbolic link to program from bin/
bin/$(program): $(outdir)$(program)
ifneq ($(MODE), analysis)
	@mkdir -p $(dir $@);
	cd bin; rm -f $(program); ln -s ../$(outdir)$(program) $(program); cd ..
endif

# Link object files into executable
$(outdir)$(program): $(objs) | $(outdir)
	@mkdir -p $(dir $@)
ifeq ($(MODE), analysis)
	echo > $@
endif
ifneq ($(MODE), analysis)
	$(CXX) $(objs) $(ldflags) -o $@
	if [ -f $@.exe ]; then \
      mv -f $@.exe $@; \
	fi
endif
ifeq ($(MODE), release)
	strip $@
endif

# Link object files into library
library: bin/$(library)
bin/$(library): $(objs) | bin/
	rm -f bin/$(library)
ifeq ($(MODE), shared)
	$(CXX) -shared -o bin/$(library) $(ldflags) \
	  $(patsubst $(outdir)main.o,,$(objs))
else
	ar crs bin/$(library) $(patsubst $(outdir)main.o,,$(objs))
endif

# Compile and output object files.
# In analysis mode no file is created, so create one
# to allow dependency analysis to work.
$(outdir)%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) ${cflags} -c $< -o $@
	$(CXX) $(cflags) -MM -c $< > $(@:.o=.d).tmp
# using /usr/bin/env echo to get the non-built-in echo on OS X, since
# the built-in one does not understand the parameter -n.
	@/usr/bin/env echo -n "$(dir $@)" > $(@:.o=.d)
	@cat $(@:.o=.d).tmp >> $(@:.o=.d)
	@sed -e 's/.*://' -e 's/\\$$//' < $(@:.o=.d).tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(@:.o=.d)
	@rm -f $(@:.o=.d).tmp
ifeq ($(MODE), analysis)
	  echo > $@
endif

-include $(objs:.o=.d)

# Installation.
install:
	if [ "`uname|grep CYGWIN`" = "" ]; then \
		sudo install bin/frobby $(BIN_INSTALL_DIR); \
	else \
		install bin/frobby $(BIN_INSTALL_DIR); \
	fi  # Cygwin has no sudo

# ***** Documentation

# We need to run latex three times to make sure that references are done
# properly in the output.
doc: docPs docPdf
docPs:
	rm -rf bin/doc
	mkdir bin/doc
	for i in 1 2 3; do latex doc/manual.tex -output-directory=bin/doc/; done
	cd bin; dvips doc/manual.dvi
docPdf:
	rm -rf bin/doc
	mkdir bin/doc
	for i in 1 2 3; do pdflatex doc/manual.tex -output-directory=bin/doc/; done
	mv bin/doc/manual.pdf bin
docDviOnce: # Useful to view changes when writing the manual
	latex doc/manual.tex -output-directory=bin/doc

# It may seem wasteful to run doxygen three times to generate three
# kinds of output. However, the latex output for creating a pdf file
# and for creating a PostScript file is different, and so at least two
# runs are necessary. Making the HTML output a third run is cleaner
# than tacking it onto one or both of the other two targets.
develDoc: develDocHtml develDocPdf develDocPs
develDocHtml:
	cat doc/doxygen.conf doc/doxHtml|doxygen -
develDocPdf:
	rm -rf bin/develDoc/latexPdf bin/develDoc/warningLog
	cat doc/doxygen.conf doc/doxPdf|doxygen -
	cd bin/develDoc/latexPdf; for f in `ls *.eps`; do epstopdf $$f; done # Cygwin fix
	cd bin/develDoc/latexPdf/; make refman.pdf; mv refman.pdf ../develDoc.pdf
develDocPs:
	rm -rf bin/develDoc/latexPs bin/develDoc/warningLog
	cat doc/doxygen.conf doc/doxPs|doxygen -
	cd bin/develDoc/latexPs/; make refman.ps; mv refman.ps ../develDoc.ps

clean: tidy
	rm -rf bin

# ***** Miscellaneous

tidy:
	find .|grep -x ".*~\|.*/\#.*\#|.*\.stackdump\|gmon\.out\|.*\.orig\|.*/core\|core"|xargs rm -f

# Convert any Windows-style whitespace into Unix-style whitespace. This is
# a good thing to do before a commit if developing on e.g. Cygwin.
dos2unix:
	for f in `find src/ test/ scr/ doc/ -type f`; do dos2unix $$f; done

commit: test
	echo
	hg commit -m "$(MSG)"

# ***** Distribution

remoteUrl = ssh://daimi/projs/frobby
pull:
	hg pull $(remoteUrl)
	cd sage; hg pull $(remoteUrl)/sage
push:
	hg push $(remoteUrl)
	cd sage; hg push $(remoteUrl)/sage

distribution:
ifndef VER
	echo "Please specify version of Frobby distribution using VER=x.y.z";
	exit 1;
endif
	rm -fr frobby_v$(VER).tar.gz frobby_v$(VER)
	mkdir frobby_v$(VER)
	cp -r frobgrob COPYING Makefile src test doc frobby_v$(VER)
	mkdir frobby_v$(VER)/4ti2
	tar --create --gzip --file=frobby_v$(VER).tar.gz frobby_v$(VER)/
	rm -fr frobby_v$(VER)	
	ls -l frobby_v$(VER).tar.gz

spkg: tidy depend
ifndef VER
	echo "Please specify version of Frobby spkg using VER=x.y.z";
	exit 1;
endif
	if [ "$$SAGE_LOCAL" = "" ]; then \
	  echo "SAGE_LOCAL undefined ... exiting"; \
	  echo "Maybe run 'sage -sh?'" \
	  exit 1; \
	fi

	if [ ! -d sage/ ]; then echo "sage/ directory not found."; exit 1; fi
# Ensure that previous builds have been cleaned up
	rm -rf bin/sagetmp bin/frobby-$(VER) bin/frobby-$(VER).spkg

	hg clone sage bin/sagetmp

	mkdir bin/sagetmp/src
	cp -r COPYING Makefile src test bin/sagetmp/src

	mv bin/sagetmp bin/frobby-$(VER)
	cd bin/; $(SAGE_ROOT)/sage -pkg `pwd`/frobby-$(VER)
	rm -rf bin/frobby-$(VER)
