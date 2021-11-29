FLAGS=-std=c++20  -fconcepts -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wconversion -Wsign-conversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -lquadmath

DEBUG=-g

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
	CC=g++-11 -I/usr/local/include -L/usr/local/lib $(FLAGS) $(DEBUG) -lfltk
endif

ifeq ($(UNAME), Linux)
	CC=g++ $(FLAGS) $(DEBUG) -lfltk
endif

OBJDIR = build

POBJ=\
	main.o\
	animation.o\
	grid.o\
	cell_content.o\
	point.o\
	game.o\
	board_state.o\
	shape.o

OBJ=$(addprefix $(OBJDIR)/, $(POBJ))

main.out : $(OBJ)
	$(CC) -o $@ $^

-include $(OBJDIR)/*.d  # include dependencies

$(OBJ): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o : %.cpp
	$(CC) -o $@ -c $< -MMD

.PHONY : clean
clean :
	rm $(OBJDIR)/*.o
