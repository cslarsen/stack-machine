CXXFLAGS = -g -Iinclude
TARGETS = instructions.o parser.o sm-util.o fileptr.o machine.o compiler.o sm smr smc smd

all: $(TARGETS)
	@echo Run \"make check\" to test package

%.sm: tests/%.src
	./smc $<

smr: instructions.o machine.o sm-util.o fileptr.o smr.cpp

smc: instructions.o machine.o sm-util.o fileptr.o parser.o compiler.o smc.cpp

smd: instructions.o machine.o sm-util.o fileptr.o smd.cpp

sm: instructions.o machine.o sm-util.o fileptr.o parser.o compiler.o sm.cpp

check: all
	./smc tests/hello-world.txt
	./smd tests/hello-world.sm
	./sm tests/hello-world.sm
	./sm tests/fib.sm | head -20
	./smc tests/hello.src
	./smr tests/hello.sm
	./smc tests/hello-world.txt
	./smr tests/hello-world.sm
	./smc tests/fib.src
	./smr tests/fib.sm
	./smc tests/forward-goto.src
	./smr tests/forward-goto.sm
	./sm tests/yo.src
	./sm tests/func.src

clean:
	rm -f $(TARGETS) *.stackdump tests/*.sm
