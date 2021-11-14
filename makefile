FLAGS=-std=c++20 -masm=intel -fconcepts -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wconversion -Wsign-conversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -lquadmath

DEBUG=-g

CC=g++ $(FLAGS) $(DEBUG) -lfltk

OBJ=\
	animation.o\
	common.o\
	point.o

main.out : $(OBJ)

-include *.d  # include dependencies

%.o : %.cpp
	$(CC) -o $@ -c $< -MMD

.PHONY : clean
clean :
	rm *.o
