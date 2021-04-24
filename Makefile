CXX = g++

ENTRY = src/main.cpp
BINARY = bin/sc
PROGRAMS = programs/
ARGS = -std=c++1z -lreadline

release:
	$(CXX) $(ENTRY) $(ARGS) -o $(BINARY)

debug:
	$(CXX) -g $(ENTRY) $(ARGS) -o $(BINARY)

programs: release
	for file in $(PROGRAMS)*.bas; do \
		$(BINARY) -b $$file $(PROGRAMS)$$(basename "$$file" .bas).asm; \
	done