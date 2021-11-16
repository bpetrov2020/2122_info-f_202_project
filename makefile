FLAGS=-std=c++20 -masm=intel -fconcepts -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wconversion -Wsign-conversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -lquadmath

DEBUG=-g

CC=g++ $(FLAGS) $(DEBUG) -lfltk

OBJDIR = build

POBJ=\
	main.o\
	grid.o\
	cell_content.o\
	point.o\
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
