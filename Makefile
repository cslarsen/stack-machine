CC = g++
CXX = g++
CXXFLAGS = -Wall -Weffc++ -g -Iinclude
TARGETS = instructions.o parser.o error.o upper.o fileptr.o machine.o compiler.o sm.o smr.o smc.o smd.o sm smr smc smd


all: $(TARGETS)
	@echo Run \"make check\" to test package

%.sm: tests/%.src
	./smc $<

smr: instructions.o machine.o upper.o fileptr.o smr.o

smc: instructions.o machine.o upper.o error.o fileptr.o parser.o compiler.o smc.o

smd: instructions.o machine.o upper.o error.o fileptr.o smd.o

sm: instructions.o machine.o upper.o error.o fileptr.o parser.o compiler.o sm.o

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
	cat tests/core-test.src tests/core.src | ./sm -

clean:
	rm -f $(TARGETS) *.stackdump tests/*.sm
