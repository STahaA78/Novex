# -------- Config --------
CXX = clang++
CXXFLAGS = -std=c++17 `llvm-config --cxxflags` -I.
LDFLAGS = `llvm-config --ldflags --libs core` -fPIE -pie
LEX = flex
YACC = bison -d

# -------- Files --------
LEXER = novex.l
PARSER = novex.y
LEX_C = lex.yy.c
PARSER_C = novex.tab.c
PARSER_H = novex.tab.h

SRCS = AST.cpp IR.cpp symbol_table.cpp $(LEX_C) $(PARSER_C)
OBJS = $(SRCS:.cpp=.o)
TARGET = novex

# -------- Build Rules --------
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(PARSER_C) $(PARSER_H): $(PARSER)
	$(YACC) $(PARSER)

$(LEX_C): $(LEXER) $(PARSER_H)
	$(LEX) $<

# Generate LLVM IR (.ll) from input and compile to native binary
run: $(TARGET)
	./$(TARGET) < input.novex
	clang output.ll -o final_exec

.PHONY: all clean run


clean:
	rm -f $(TARGET) *.o lex.yy.c novex.tab.* output.ll final_exec


