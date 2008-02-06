# ***** Variables

rawSources =															\
  main.cpp Action.cpp IOParameters.cpp									\
  IrreducibleDecomAction.cpp fplllIO.cpp IOHandler.cpp fourti2.cpp		\
  randomDataGenerators.cpp MonosIOHandler.cpp BigIdeal.cpp				\
  TransformAction.cpp													\
  Macaulay2IOHandler.cpp NewMonosIOHandler.cpp HelpAction.cpp			\
  stdinc.cpp DynamicFrobeniusAction.cpp dynamicFrobeniusAlgorithm.cpp	\
  GenerateIdealAction.cpp GenerateFrobeniusAction.cpp					\
  IrreducibleDecomFacade.cpp FrobeniusAction.cpp Facade.cpp				\
  IOFacade.cpp DynamicFrobeniusFacade.cpp GenerateDataFacade.cpp		\
  AnalyzeAction.cpp IdealFacade.cpp Parameter.cpp						\
  ParameterGroup.cpp IntegerParameter.cpp								\
  IrreducibleDecomParameters.cpp BoolParameter.cpp						\
  Scanner.cpp Partition.cpp StringParameter.cpp Term.cpp				\
  TermTranslator.cpp Timer.cpp VarNames.cpp LatticeFormatAction.cpp		\
  SliceAlgorithm.cpp													\
  Ideal.cpp intersect.cpp IntersectFacade.cpp IntersectAction.cpp		\
  AssociatedPrimesFacade.cpp AssociatedPrimesAction.cpp					\
  PrimaryDecomAction.cpp Slice.cpp										\
  IndependenceSplitter.cpp Projection.cpp								\
  SliceStrategy.cpp lattice.cpp											\
  LatticeFacade.cpp DecomRecorder.cpp									\
  TermGrader.cpp Fourti2IOHandler.cpp NullIOHandler.cpp Minimizer.cpp	\
  AlexanderDualAction.cpp

ldflags = -lgmpxx -lgmp
cflags = -Wall -ansi -pedantic -Wextra -Wno-uninitialized	\
         -Wno-unused-parameter -Werror 

ifndef MODE
 MODE=release
endif

ifeq ($(MODE), release)
  outdir = bin/release/
  cflags += -O3
else ifeq ($(MODE), debug)
  outdir = bin/debug/
  cflags += -g -D DEBUG -fno-inline
else ifeq ($(MODE), profile)
  outdir = bin/profile/
  cflags += -g -pg -O3
  ldflags += -pg
else ifeq ($(MODE), analysis)
  outdir = bin/analysis/
  cflags += -fsyntax-only -O1 -Wfloat-equal -Wundef			\
            -Wno-endif-labels -Wshadow -Wlarger-than-1000		\
            -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings	\
            -Wconversion -Wsign-compare -Waggregate-return		\
            -Wmissing-noreturn -Wmissing-format-attribute		\
            -Wno-multichar -Wno-deprecated-declarations -Wpacked	\
            -Wredundant-decls -Wunreachable-code -Winline		\
            -Wno-invalid-offsetof -Winvalid-pch -Wlong-long		\
            -Wdisabled-optimization
else
  $(error Unknown value of MODE: "$(MODE)")
endif

sources = $(patsubst %.cpp, src/%.cpp, $(rawSources))
objs    = $(patsubst %.cpp, $(outdir)%.o, $(rawSources))
program = frobby

# ***** Compilation

.PHONY: all depend clean bin/$(program)
all: bin/$(program) $(outdir)$(program)
ifeq ($(MODE), profile)
	rm -f gmon.out
	./bench
	gprof bin/frobby > prof
endif

test: all
ifdef TESTCASE
	export frobby=bin/$(program); echo; echo -n "$(TESTCASE): " ; \
	cd test/$(TESTCASE); ./runtests $(TESTARGS); cd ../..
else
	export frobby=bin/$(program); ./test/runtests
endif

bench: all
	cd data;time ./runbench $(OPTS)

$(outdir): $(outdir)label
	mkdir -p $(outdir)
$(outdir)label:
	mkdir -p $(outdir)label


# Make symbolic link to program from bin/
bin/$(program): $(outdir)$(program)
ifneq ($(MODE), analysis)
	cd bin; cp -sf ../$(outdir)$(program) $(program); cd ..
endif

# Link object files into executable
$(outdir)$(program): $(objs) | $(outdir)
ifneq ($(MODE), analysis)
	g++ $(objs) $(ldflags) -o $(outdir)$(program)
	mv -f $(outdir)$(program).exe $(outdir)$(program); echo

endif
ifeq ($(MODE), release)
	strip $(outdir)$(program)
endif


# Compile and output object files.
# In analysis mode no file is created, so create one
# to allow dependency analysis to work.
$(outdir)%.o: src/%.cpp | $(outdir)
	  g++ ${cflags} -c $< -o $(outdir)$(subst src/,,$(<:.cpp=.o))
ifeq ($(MODE), analysis)
	  echo > $(outdir)$(subst src/,,$(<:.cpp=.o))
endif

# ***** Dependency management
depend:
	g++ -MM $(sources) | sed 's/^[^\ ]/$$(outdir)&/' > .depend
-include .depend

clean: tidy
	rm -rf bin frobby_v*.tar.gz

tidy:
	find -name "*~" -exec rm -f {} \;
	find -name "*.stackdump" -exec rm -f {} \;
	find -name "gmon.out" -exec rm -f {} \;
	find -name "*.orig" -exec rm -f {} \;
	find -name "core" -exec rm -f {} \;

# ***** Mercurial

commit: test
	echo
	hg commit -m "$(MSG)"

# ***** Distribution

distribution:
	make depend
	make realdistribution VER="$(VER)"

fastdistribution: tidy
	cd ..;tar --create --file=frobby_v$(VER).tar.gz frobby/ --gzip \
	  --exclude=*/data/* --exclude=*/data \
	  --exclude=*/.hg/* --exclude=*/.hg --exclude=.hgignore\
	  --exclude=*/bin/* --exclude=*/bin \
	  --exclude=*/save/* --exclude=*/save \
	  --exclude=*/4ti2/* \
	  --exclude=*.tar.gz \
      --exclude=*/web/* \
      --exclude=*/web \
      --exclude=*/tmp/* \
      --exclude=pullf \
      --exclude=pushf \
      --exclude=sync \
      --exclude=*~ --exclude=*.orig \
      --exclude=gmon.out \
      --exclude=*.stackdump
	mv ../frobby_v$(VER).tar.gz .
	ls -l frobby_v$(VER).tar.gz

realdistribution: tidy
	rm -fr frobby_v$(VER).tar.gz frobby_v$(VER)
	mkdir frobby_v$(VER)
	cp -r COPYING Makefile src test frobby_v$(VER)
	mkdir frobby_v$(VER)/4ti2
	./addheaders `find frobby_v$(VER)/src|grep "\(.*\.h\)\|\(.*.cpp\)"`
	tar --create --gzip --file=frobby_v$(VER).tar.gz frobby_v$(VER)/
	rm -fr frobby_v$(VER)	
	ls -l frobby_v$(VER).tar.gz
