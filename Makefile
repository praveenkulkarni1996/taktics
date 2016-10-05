CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -w    disables warnings
CFLAGS  = -w -Ofast -march=native -std=c++11

# the build target executable:
TARGET = player
CPPFILES = player.cpp board.cpp utility.cpp

all: $(TARGET)

$(TARGET): $(CPPFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(CPPFILES)
	cp $(TARGET) /media/quantumcoder/OS/Users/quantumcoder/Desktop/iitd/Sem5/AI/assignments/assignment3/Tak-sim/ 

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
