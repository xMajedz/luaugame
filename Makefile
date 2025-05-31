CXX=g++
CXX_FLAGS=-g -Wall
SOURCES=sources/main.cpp
OBJECTS=build/main.cpp.o
TARGET=build/luaugame

LUAU_INC_DIRS=-Iluau/Compiler/include -Iluau/VM/include
RL_INC_DIR=-Iraylib/src
INC_DIRS=$(LUAU_INC_DIRS) $(RL_INC_DIR)

LUAU_LIBS_DIR=luau/build/release
RL_LIB_DIR=-Lraylib/src
LIB_DIRS=$(RL_LIB_DIR)

COMPILER_TARGET=$(LUAU_LIBS_DIR)/libluaucompiler.a
AST_TARGET=$(LUAU_LIBS_DIR)/libluauast.a
VM_TARGET=$(LUAU_LIBS_DIR)/libluauvm.a

LUAU_LIBS=$(COMPILER_TARGET) $(AST_TARGET) $(VM_TARGET)
RL_LIB=-lraylib
LIBS=$(LUAU_LIBS) $(RL_LIB)

CXXFLAGS+=-O2 -DNDEBUG -fno-math-errno
CXX+=$(CXX_FLAGS)

$(TARGET): $(OBJECTS) 
	$(CXX) $< $(LUAU_LIBS) $(LIBS) $(LIB_DIRS) $(INC_DIRS) -o $@

$(OBJECTS): $(SOURCES)
	$(CXX) $< -c -MMD -MP $(INC_DIRS) -o $@
