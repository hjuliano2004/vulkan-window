# Compiladores
CC = gcc
CXX = g++

# Flags de compilação
CFLAGS   = -Wall -O2
CXXFLAGS = -Wall -O2

# Busca todos os arquivos .c e .cpp recursivamente
SRC_C   := $(shell find . -name '*.c')
SRC_CPP := $(shell find . -name '*.cpp')
SRC     := $(SRC_C) $(SRC_CPP)

# Objetos correspondentes
OBJ     := $(SRC:.c=.o)
OBJ     := $(OBJ:.cpp=.o)

# Nome do executável
TARGET = run

# Regra principal
all: $(TARGET)
	clear
	./$(TARGET)

# Linkagem final (com Wayland via pkg-config)
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(shell pkg-config --libs wayland-client)

# Regras de compilação
%.o: %.c
	$(CC) $(CFLAGS) $(shell pkg-config --cflags wayland-client) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(shell pkg-config --cflags wayland-client) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJ) $(TARGET)
