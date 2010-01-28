TARGETS = sm-core.o sm-run sm-compile sm-dis

all: $(TARGETS) check

sm-run: sm-core.o sm-run.cpp

sm-compile: sm-core.o sm-compile.cpp

sm-dis: sm-core.o sm-dis.cpp

check:
	./sm-run hello-world.sm

clean:
	rm -f $(TARGETS)
