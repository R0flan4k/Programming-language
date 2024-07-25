CXX = g++
CXXFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -DNDEBUG -D_EJUDGE_CLIENT_SIDE -Wno-parentheses
IFLAGS = -I./include -I../lib -I../syntaxis/include
OBJDIR = ./obj
SRCDIR = ./src
LIBDIR = ../lib
SYNTAXISDIR = ../syntaxis/src
# SRC = $(SRCDIR)/*.cWpp
# OBJ = $(OBJDIR)/*.o $(LIBDIR)/*.o
SRC = $(wildcard $(SRCDIR)/*.cpp) $(LIBDIR)/*.cpp $(SYNTAXISDIR)/*.cpp
OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
CXXFLAGS += $(IFLAGS)
OUT = front_end

all : $(OBJ)
	@$(CXX) $(IFLAGS) $(CFLAGS) $^ -o $(OUT)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(IFLAGS) $(CXXFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	@rm -f $(OBJDIR)/*.o ./graphviz/*.dot ./graphviz/*.png ./latex/* *.exe $(OUT)
