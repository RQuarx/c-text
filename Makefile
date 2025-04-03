CXX=clang++
LD=clang++

PKGCONFIG=$(shell which pkg-config)
DEPS=sdl3 sdl3-ttf fontconfig

CXXFLAGS=-std=c++23 -O3 -Wall -Wextra -Iinc $(shell $(PKGCONFIG) --cflags $(DEPS))
LDFLAGS=-fuse-ld=lld $(shell $(PKGCONFIG) --libs $(DEPS))

SRC=$(shell find src/ -name '*.cpp')
BUILDDIR=target
BUILD=$(BUILDDIR)/c+text

OBJDIR=$(BUILDDIR)/c+text.o
OBJ=$(patsubst src/%.cpp, $(OBJDIR)/%.cpp.o, $(SRC))

BIN=/usr/bin
APP=/usr/share/applications

all:
	@echo "Use 'make setup' and 'make compile'"

setup:
	@mkdir -p $(OBJDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

compile: $(BUILDDIR) $(OBJDIR) $(OBJ)
	@$(LD) $(LDFLAGS) -o $(BUILD) $(OBJ)

$(OBJDIR)/%.cpp.o: src/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: all setup compile clean

MAKEFLAGS+=-j$(shell nproc)