TESTFILES := $(wildcard *.crona)
TESTS := $(TESTFILES:.crona=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@echo "Testing $*.crona"
	@touch $*.err #The @ means don't show the command being invoked
	@../cronac $*.crona -c 2> $*.err ;\
	PROG_EXIT_CODE=$$?;\
	echo "diff error...";\
	diff $*.err $*.err.expected;\
	ERR_EXIT_CODE=$$?;\
	exit $$ERR_EXIT_CODE

clean:
	rm -f *.out *.err
