# Flags used when compiling C++.
CXXFLAGS += -Wall -Wextra -std=c++17
opt: CXXFLAGS += -ffunction-sections -fdata-sections -flto -O3
debug: CXXFLAGS += -O0 -g

# Flags used when linking.
LDFLAGS += -fuse-ld=gold
opt: LDFLAGS += -s -Wl,--gc-sections

# Libraries that should be linked.
LDLIBS =  \
	-lX11  \
	-lasound

# Installation path prefix.
INSTALL_PATH = /usr/bin
