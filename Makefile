vpath %.s src/coroutine
vpath %.cc src src/coroutine
# Makefile内容
SRC_PATH := src src/coroutine
OBJ_PATH = build
#SRC=./src/main.cc ./src/coroutine/coroutine_context.cc ./src/coroutine/context_swap.s
SRC=$(foreach path, $(SRC_PATH), $(wildcard $(addprefix $(path)/*, .cc .s)))
OBJ=$(addprefix $(OBJ_PATH)/, $(addsuffix .o,$(notdir $(basename $(SRC)))))
#SRCS=./src/main.cc ./src/coroutine/coroutine_context.cc ./src/coroutine/context_swap.s
#OBJS=./build/main.o ./build/coroutine_context.o ./build/context_swap.o
CUR_DIR  = $(shell pwd)
#D_OBJ=./build
#OBJS2=$(SRCS:.cc=.o)
#OBJS1=$(OBJS2:.s=.o)
#OBJS=$(addprefix $(D_OBJ)/, $(notdir $(OBJS1)))

$(info "SRC:$(SRC)")
$(info "OBJ:$(OBJ)")

$(shell mkdir -p $(CUR_DIR)/$(OBJ_PATH))

TARGET=context_test

$(OBJ_PATH)/$(TARGET) : $(OBJ)
	g++ $^ -o $@ -std=c++11 -w

$(OBJ_PATH)/%.o : %.cc
	g++ -c $< -o $@
$(OBJ_PATH)/%.o : %.s
	g++ -c $< -o $@
	
#./build/main.o : ./src/main.cc
	#g++ -c ./src/main.cc -o ./build/main.o
#./build/coroutine_context.o : ./src/coroutine/coroutine_context.cc
	#g++ -c ./src/coroutine/coroutine_context.cc -o ./build/coroutine_context.o
#./build/context_swap.o : ./src/coroutine/context_swap.s
	#g++ -c ./src/coroutine/context_swap.s -o ./build/context_swap.o
cleanobj:
	rm -rf $(OBJ_PATH)/*.o
cleanall:
	rm -rf $(OBJ_PATH)/*.o $(OBJ_PATH)/$(TARGET)
#main.o : ./src/main.cc
	#g++ -c ./src/main.cc -o main.o
#coroutine_context.o : ./src/coroutine/coroutine_context.cc
	#g++ -c ./src/coroutine/coroutine_context.cc -o coroutine_context.o
#context_swap.o : ./src/coroutine/context_swap.s
	#g++ -c ./src/coroutine/context_swap.s -o context_swap.o
    

