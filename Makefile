TARGETS = sm-core.o sm-run sm-compile sm-dis

all: $(TARGETS) check

sm-run: sm-core.o sm-run.cpp

sm-compile: sm-core.o sm-compile.cpp

sm-dis: sm-core.o sm-dis.cpp

check:
	./sm-dis hello-world.sm
	./sm-run hello-world.sm
	./sm-compile
	./sm-run fib.sm | head -20

clean:
	rm -f $(TARGETS)
