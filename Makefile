TARGETS = sm-core.o sm-run sm-compile sm-dis

all: $(TARGETS)
	@echo Run \"make check\" to test package

sm-run: sm-core.o sm-run.cpp

sm-compile: sm-core.o sm-compile.cpp

sm-dis: sm-core.o sm-dis.cpp

check: all
	./sm-dis hello-world.sm
	./sm-run hello-world.sm
	./sm-run fib.sm | head -20
	./sm-compile hello.src
	./sm-run hello.src.sm
	./sm-compile hello-world.txt
	./sm-run hello-world.txt.sm
	./sm-compile fib.src
	./sm-run fib.src.sm
	./sm-compile forward-goto.src
	./sm-run forward-goto.src.sm

clean:
	rm -f $(TARGETS)
