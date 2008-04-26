# ***** Variables

rawSources = main.cpp Action.cpp IOParameters.cpp                       \
  IrreducibleDecomAction.cpp fplllIO.cpp IOHandler.cpp fourti2.cpp      \
  randomDataGenerators.cpp MonosIOHandler.cpp BigIdeal.cpp              \
  TransformAction.cpp Macaulay2IOHandler.cpp NewMonosIOHandler.cpp      \
  HelpAction.cpp stdinc.cpp DynamicFrobeniusAction.cpp                  \
  dynamicFrobeniusAlgorithm.cpp GenerateIdealAction.cpp                 \
  GenerateFrobeniusAction.cpp IrreducibleDecomFacade.cpp                \
  FrobeniusAction.cpp Facade.cpp IOFacade.cpp                           \
  DynamicFrobeniusFacade.cpp GenerateDataFacade.cpp AnalyzeAction.cpp   \
  IdealFacade.cpp Parameter.cpp ParameterGroup.cpp                      \
  IntegerParameter.cpp IrreducibleDecomParameters.cpp                   \
  BoolParameter.cpp Scanner.cpp Partition.cpp StringParameter.cpp       \
  Term.cpp TermTranslator.cpp Timer.cpp VarNames.cpp                    \
  LatticeFormatAction.cpp SliceAlgorithm.cpp Ideal.cpp intersect.cpp    \
  IntersectFacade.cpp IntersectAction.cpp AssociatedPrimesFacade.cpp    \
  AssociatedPrimesAction.cpp PrimaryDecomAction.cpp Slice.cpp           \
  IndependenceSplitter.cpp Projection.cpp SliceStrategy.cpp             \
  lattice.cpp LatticeFacade.cpp DecomRecorder.cpp TermGrader.cpp        \
  Fourti2IOHandler.cpp NullIOHandler.cpp Minimizer.cpp                  \
  AlexanderDualAction.cpp frobby.cpp BigTermConsumer.cpp                \
  TranslatingTermConsumer.cpp frobbyTest.cpp


GMP_INC_DIR="/sw/include"

ldflags = -lgmpxx -lgmp -L/sw/lib
cflags = -Wall -ansi -pedantic -Wextra -Wno-uninitialized \
         -Wno-unused-parameter -Werror -I$(GMP_INC_DIR)

ifndef MODE
 MODE=release
endif

MATCH=false
ifeq ($(MODE), release)
  outdir = bin/release/
  cflags += -O3
  MATCH=true
endif
ifeq ($(MODE), debug)
  outdir = bin/debug/
  cflags += -g -D DEBUG -fno-inline
  MATCH=true
endif
ifeq ($(MODE), profile)
  outdir = bin/profile/
  cflags += -g -pg -O3
  ldflags += -pg
  MATCH=true
endif
ifeq ($(MODE), analysis)
  outdir = bin/analysis/
  cflags += -fsyntax-only -O1 -Wfloat-equal -Wundef                     \
            -Wno-endif-labels -Wshadow -Wlarger-than-1000               \
            -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings    \
            -Wconversion -Wsign-compare -Waggregate-return              \
            -Wmissing-noreturn -Wmissing-format-attribute               \
            -Wno-multichar -Wno-deprecated-declarations -Wpacked        \
            -Wredundant-decls -Wunreachable-code -Winline               \
            -Wno-invalid-offsetof -Winvalid-pch -Wlong-long             \
            -Wdisabled-optimization
  MATCH=true
endif

ifeq ($(MATCH), false)
  $(error Unknown value of MODE: "$(MODE)")
endif

sources = $(patsubst %.cpp, src/%.cpp, $(rawSources))
objs    = $(patsubst %.cpp, $(outdir)%.o, $(rawSources))
program = frobby
library = frobby.a

# ***** Compilation

.PHONY: all depend clean bin/$(program) test library

# to make .depend able to mention these files  on other platforms.
.PHONY: /sw/include/gmp.h /sw/include/gmpxx.h

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
	cd bin; rm -f $(program); link ../$(outdir)$(program) $(program); cd ..
endif

# Link object files into executable
$(outdir)$(program): $(objs) | $(outdir)
ifeq ($(MODE), analysis)
	echo > $(outdir)$(program)
endif
ifneq ($(MODE), analysis)
	g++ $(objs) $(ldflags) -o $(outdir)$(program)
	if [ -f $(outdir)$(program).exe ]; then \
	  mv -f $(outdir)$(program).exe $(outdir)$(program); \
	fi
endif
ifeq ($(MODE), release)
	strip $(outdir)$(program)
endif

# Link object files into static library
library: $(outdir)$(library)
$(outdir)$(library): $(objs) | $(outdir)
ifneq ($(MODE), analysis)
	rm -f $(outdir)$(library)
	ar r $(outdir)$(library) $(patsubst main.o, , $(objs))
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
	g++ ${cflags} -MM $(sources) | sed 's/^[^\ ]/$$(outdir)&/' > .depend
-include .depend

clean: tidy
	rm -rf bin frobby_v*.tar.gz

tidy:
	find .|grep -x ".*~\|.*\.stackdump\|gmon\.out\|.*\.orig\|.*/core\|core"|xargs rm -f

# ***** Mercurial

commit: test
	echo
	hg commit -m "$(MSG)"

# ***** Distribution

remoteCmd = --remotecmd /users/contrib/mercurial/bin/hg
remoteUrl = ssh://daimi/projs/frobby
pull:
	hg pull $(remoteCmd) $(remoteUrl)
push:
	hg push $(remoteCmd) $(remoteUrl)


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
	cp -r frobgrob COPYING Makefile src test frobby_v$(VER)
	mkdir frobby_v$(VER)/4ti2
	./addheaders `find frobby_v$(VER)/src|grep "\(.*\.h\)\|\(.*.cpp\)"`
	tar --create --gzip --file=frobby_v$(VER).tar.gz frobby_v$(VER)/
	rm -fr frobby_v$(VER)	
	ls -l frobby_v$(VER).tar.gz
