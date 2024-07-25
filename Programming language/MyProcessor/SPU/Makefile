CC = g++
FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE
IFLAGS = -Iinclude -I../lib/include -I../MyStack/include -I../config
STKDIR = ../MyStack
SRCDIR = ./src
OBJDIR = ./obj
CONFIGDIR = ../config
LIBDIR = ../lib
SPU = spu
FILEPROCESSING = file_processing
ARITHMETIC = commands_processing
STACK = stack
VERIFICATOR = vtor
MYASSERT = my_assert
CMDINPUT = cmd_input
HASH = hash
PFLAGS = flags
STACKHASH = stack_hash
DOUBLECOMPARING = double_comparing
OBJECTS = $(OBJDIR)/$(SPU).o $(OBJDIR)/$(STACK).o $(OBJDIR)/$(VERIFICATOR).o $(OBJDIR)/$(MYASSERT).o $(OBJDIR)/$(CMDINPUT).o $(OBJDIR)/$(HASH).o $(OBJDIR)/$(ARITHMETIC).o $(OBJDIR)/$(FILEPROCESSING).o $(OBJDIR)/$(PFLAGS).o $(OBJDIR)/$(STACKHASH).o $(OBJDIR)/$(DOUBLECOMPARING).o

all: $(OBJECTS)
	@$(CC) $(OBJECTS) -o spu

$(OBJDIR)/$(SPU).o: $(SRCDIR)/$(SPU).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(ARITHMETIC).o: $(SRCDIR)/$(ARITHMETIC).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(STACK).o: $(STKDIR)/$(STACK).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(VERIFICATOR).o: $(STKDIR)/$(VERIFICATOR).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(CMDINPUT).o: $(LIBDIR)/$(CMDINPUT).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(PFLAGS).o: $(SRCDIR)/$(PFLAGS).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(STACKHASH).o: $(STKDIR)/$(STACKHASH).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(FILEPROCESSING).o: $(LIBDIR)/$(FILEPROCESSING).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(MYASSERT).o: $(LIBDIR)/$(MYASSERT).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(HASH).o: $(LIBDIR)/$(HASH).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@

$(OBJDIR)/$(DOUBLECOMPARING).o: $(LIBDIR)/$(DOUBLECOMPARING).cpp
	@$(CC) $(FLAGS) $(IFLAGS) -c $^  -o $@
clean:
	@rm -f $(OBJDIR)/*.o *~ core *~ *.exe
