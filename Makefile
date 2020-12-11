GPP = g++

options = -Wall -Wextra

src = src
bin = bin

input = input.txt

run : $(bin)\compilier.exe
	$(bin)\compilier.exe $(input)

$(bin)\compilier.exe : $(bin)\parser.o $(bin)\tree.o $(src)\main.cpp
	$(GPP) $(src)\main.cpp $(bin)\parser.o $(bin)\tree.o -o $(bin)\compilier.exe $(options)

$(bin)\parser.o : $(src)\tree\parser.cpp $(src)\tree\parser.h
	$(GPP) -c $(src)\tree\parser.cpp -o $(bin)\parser.o $(options)

$(bin)\tree.o : $(src)\tree\tree.cpp $(src)\tree\tree.h $(src)\tree\parser.h $(src)\tree\dot_dump.h
	$(GPP) -c $(src)\tree\tree.cpp -o $(bin)\tree.o  $(options)
