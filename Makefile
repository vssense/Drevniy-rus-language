GPP = g++

options = -Wall -Wextra -w

src = src
bin = bin

input = input.txt

run : $(bin)\compilier.exe
	$(bin)\compilier.exe $(input)

$(bin)\compilier.exe : $(bin)\parser.o $(bin)\tree.o 
	$(GPP) $(src)\main.cpp $(bin)\parser.o $(bin)\tree.o -o $(bin)\compilier.exe $(options)

$(bin)\parser.o : $(src)\parser.cpp $(src)\parser.h
	$(GPP) -c $(src)\parser.cpp -o $(bin)\parser.o $(options)

$(bin)\tree.o : $(src)\tree.cpp $(src)\tree.h
	$(GPP) -c $(src)\tree.cpp -o $(bin)\tree.o  $(options)
