CXXFLAGS = -g -Iinclude
TARGETS = sm-core.o parser.o sm-util.o fileptr.o machine.o compiler.o sm-go sm-run sm-compile sm-dis core.sm

all: $(TARGETS)
	@echo Run \"make check\" to test package

%.sm: tests/%.src
	./sm-compile $<

sm-run: sm-core.o machine.o sm-util.o fileptr.o sm-run.cpp

sm-compile: sm-core.o machine.o sm-util.o fileptr.o parser.o compiler.o sm-compile.cpp

sm-dis: sm-core.o machine.o sm-util.o fileptr.o sm-dis.cpp

sm-go: sm-core.o machine.o sm-util.o fileptr.o parser.o compiler.o sm-go.cpp

check: all
	./sm-compile tests/hello-world.txt
	./sm-dis tests/hello-world.sm
	./sm-run tests/hello-world.sm
	./sm-run tests/fib.sm | head -20
	./sm-compile tests/hello.src
	./sm-run tests/hello.sm
	./sm-compile tests/hello-world.txt
	./sm-run tests/hello-world.sm
	./sm-compile tests/fib.src
	./sm-run tests/fib.sm
	./sm-compile tests/forward-goto.src
	./sm-run tests/forward-goto.sm
	./sm-go tests/yo.src
	./sm-go tests/func.src
	./sm-run tests/core.sm

clean:
	rm -f $(TARGETS) *.stackdump tests/*.sm
