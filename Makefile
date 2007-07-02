# ***** Variables

sources = main.cpp TermTree.cpp Action.cpp				\
  IrreducibleDecomAction.cpp fplllIO.cpp io.cpp fourti2.cpp		\
  randomDataGenerators.cpp monosIO.cpp BigIdeal.cpp FormatAction.cpp	\
  macaulay2IO.cpp newMonosIO.cpp HelpAction.cpp				\
  stdinc.cpp DynamicFrobeniusAction.cpp dynamicFrobeniusAlgorithm.cpp	\
  GenerateIdealAction.cpp GenerateFrobeniusAction.cpp			\
  IrreducibleDecomFacade.cpp FrobeniusAction.cpp Facade.cpp		\
  IOFacade.cpp DynamicFrobeniusFacade.cpp RandomDataFacade.cpp		\
  AnalyzeAction.cpp AnalyzeFacade.cpp Parameter.cpp			\
  ParameterGroup.cpp GenerateIdealParameters.cpp IntegerParameter.cpp	\
  IrreducibleDecomParameters.cpp BoolParameter.cpp LabelAlgorithm.cpp	\
  Lexer.cpp Partition.cpp StringParameter.cpp Term.cpp TermList.cpp	\
  TermTranslator.cpp Timer.cpp VarNames.cpp Strategy.cpp		\
  PrintDebugStrategy.cpp FrobeniusStrategy.cpp BenchmarkStrategy.cpp	\
  DecompositionStrategy.cpp StatisticsStrategy.cpp			\
  CompositeStrategy.cpp PrintProgressStrategy.cpp			\
  SkipRedundantStrategy.cpp LatticeFormatAction.cpp

ldflags = -lgmpxx -lgmp
cflags = -Wall -ansi -pedantic -Wextra -Wno-uninitialized -Wno-unused-parameter

ifndef MODE
 MODE=release
endif

ifeq ($(MODE), release)
  outdir = bin/release/
  cflags += -O2
else ifeq ($(MODE), debug)
  outdir = bin/debug/
  cflags += -g -D DEBUG -fno-inline
else ifeq ($(MODE), profile)
  outdir = bin/profile/
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

objs    = $(patsubst %.cpp, $(outdir)%.o, $(sources))
program = frobby.exe

# ***** Targets

.PHONY: all depend clean
all: bin/$(program) $(outdir)$(program)
test: all
	export frobby=bin/$(program); ./test/runtests

$(outdir):
	mkdir -p $(outdir)

# Make symbolic link to program from bin/
bin/$(program): $(outdir)$(program)
ifneq ($(MODE), analysis)
	cd bin; cp -sf ../$(outdir)$(program) $(program); cd ..
endif

# Link object files into executable
$(outdir)$(program): $(objs) | $(outdir)
ifneq ($(MODE), analysis)
	g++ $(objs) $(ldflags) -o $(outdir)$(program)
endif

# Compile and output object files
$(outdir)%.o: %.cpp | $(outdir)
	  g++ ${cflags} -c $< -o $(outdir)$(<:.cpp=.o)
ifeq ($(MODE), analysis)
	  echo > $(outdir)$(<:.cpp=.o)
endif

# ***** Dependency management
depend:
	g++ -MM $(sources) | sed 's/^[^\ ]/$$(outdir)&/' > .depend
-include .depend

clean:
	rm -rf bin *~ *.orig
