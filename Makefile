
CC=g++ 
CFLAGS = -g --std=c++17 -I.
SOURCES=$(wildcard *.cpp)
DEPEND := $(patsubst %.d, output/%.d, $(SOURCES:.cpp=.d))
BINS := $(patsubst %_bin, output/%, $(SOURCES:.cpp=_bin))

.PHONY: all bin

all: output ${BINS}
	@echo "Done"

.PHONY: clean 
clean: 
	@rm -rf output

output:
	@mkdir -p output 


-include $(DEPEND)


output/%: %.cpp
	$(CC) $(CFLAGS) -MD $< -o $@

