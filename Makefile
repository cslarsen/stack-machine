TARGETS = sm-core.o sm-run sm-compile sm-dis

all: $(TARGETS) check

sm-run: sm-core.o sm-run.cpp

sm-compile: sm-core.o sm-compile.cpp

sm-dis: sm-core.o sm-dis.cpp

check:
	./sm-dis hello-world.sm
	./sm-run hello-world.sm
	./sm-run fib.sm | head -20
	./sm-compile hello.src
	./sm-run hello.src.sm

clean:
	rm -f $(TARGETS)
