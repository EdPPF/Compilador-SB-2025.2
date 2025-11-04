# Define o compilador:
CXX = g++

# Define as flags do compilador:
CXXFLAGS = -std=c++17 -Wall

# Define o nome do executável:
TARGET = compilador

# Define os arquivos fonte e objeto:
SOURCES = .\src\main.cpp .\src\Pre_Processador.cpp .\src\compilador.cpp .\src\Parser.cpp .\src\Tabelas.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Regra principal: diz que o TARGET depende dos arquivos objeto
all: $(TARGET)
# Regra para linkar
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Regra genérica para compilar arquivos .cpp em .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar os arquivos objeto e o executável
clean:
	rm -f $(OBJECTS) $(TARGET)