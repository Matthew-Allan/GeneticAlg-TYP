prog_name := farts

out := out
src := src

src_tests := $(src)/tests
out_tests := $(out)/tests

core := $(src)/core
common := $(src)/common

exe := $(out)/$(prog_name)

CORE_FILES := main tester loader genalg

all:
	$(MAKE) list
	
	gcc -g -lm -I$(common) $(foreach file, $(CORE_FILES), $(core)/$(file).c) -o $(exe)

run:
	./$(exe)

debug:
	gdb $(exe)

clean:
	rm -r $(out)

%:
	mkdir -p $(out_tests)/$@/imps

	$(foreach file, $(wildcard $(src_tests)/$@/imps/*.c), gcc -g -I$(src_tests)/$@ $(file) -shared -o $(out_tests)/$@/imps/$(basename $(notdir $(file))).so;)

	gcc -g -I$(common) $(src_tests)/$@/$@.c -shared -o $(out_tests)/$@/$@.so