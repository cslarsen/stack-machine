CXXFLAGS = -g
TARGETS = sm-core.o sm-util.o sm-gencode.o sm-go sm-run sm-compile sm-dis core.sm

all: $(TARGETS)
	@echo Run \"make check\" to test package

%.sm: %.src
	./sm-compile $<

sm-run: sm-core.o sm-util.o sm-run.cpp

sm-compile: sm-core.o sm-util.o sm-gencode.o sm-compile.cpp

sm-dis: sm-core.o sm-util.o sm-dis.cpp

sm-go: sm-core.o sm-util.o sm-gencode.o sm-go.cpp

check: all
	./sm-compile hello-world.txt
	./sm-dis hello-world.sm
	./sm-run hello-world.sm
	./sm-run fib.sm | head -20
	./sm-compile hello.src
	./sm-run hello.sm
	./sm-compile hello-world.txt
	./sm-run hello-world.sm
	./sm-compile fib.src
	./sm-run fib.sm
	./sm-compile forward-goto.src
	./sm-run forward-goto.sm
	./sm-go yo.src
	./sm-go func.src
	./sm-run core.sm

clean:
	rm -f $(TARGETS) *.stackdump *.sm
