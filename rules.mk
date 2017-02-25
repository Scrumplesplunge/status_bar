.PHONY: default
default: all

# Build all dependencies. This should be defined in Makefile.
.PHONY: all

# Install to the system.
.PHONY: install

# Build with optimized configuration.
.PHONY: opt
opt: all

# Build with debug configuration.
.PHONY: debug
debug: all

# Delete all generated files.
.PHONY: clean
clean:
	rm -rf bin obj

# Create the output (binary) directory.
bin:
	mkdir bin

# Create the intermediate (object) directory.
obj:
	mkdir obj

# Pattern rule for compiling a cc file into an o file.
obj/%.o: src/%.cc $(wildcard src/%.h) | obj
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -c $< -o $@

# Pattern rule for generating a binary.
bin/%: | bin
	${CXX} ${LDFLAGS} $^ -o $@ ${LDLIBS}
