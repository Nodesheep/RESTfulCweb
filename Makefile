vpath %.s src/coroutine
vpath %.cc src src/co_tcpserver src/tcpserver src/util src/cweb src/cweb/jsoncpp src/log
# Makefile内容
SRC_PATH := src src/tcpserver src/tcpserver/base src/util src/cweb src/cweb/jsoncpp src/log
OBJ_PATH = build
#SRC=./src/main.cc ./src/coroutine/coroutine_context.cc ./src/coroutine/context_swap.s
SRC=$(foreach path, $(SRC_PATH), $(wildcard $(addprefix $(path)/*, .cc .s)))
OBJ=$(addprefix $(OBJ_PATH)/, $(addsuffix .o,$(notdir $(basename $(SRC)))))
#SRCS=./src/main.cc ./src/coroutine/coroutine_context.cc ./src/coroutine/context_swap.s
#OBJS=./build/main.o ./build/coroutine_context.o ./build/context_swap.o
include_paths := src/tcpserver/base src/co_tcpserver src/tcpserver src/util src/cweb src/cweb/jsoncpp src/log \
				thirdparty/boost/1.79.0_1/include/boost

library_paths := thirdparty/boost/1.79.0_1/lib

include_paths := $(include_paths:%=-I%)
library_paths := $(library_paths:%=-L%)

compile_flags := -std=c++11 -w -g -O0 $(include_paths)
link_flags 	  := -pthread $(library_paths)

CUR_DIR  = $(shell pwd)
#D_OBJ=./build
#OBJS2=$(SRCS:.cc=.o)
#OBJS1=$(OBJS2:.s=.o)
#OBJS=$(addprefix $(D_OBJ)/, $(notdir $(OBJS1)))

$(info "SRC:$(SRC)")
$(info "OBJ:$(OBJ)")

$(shell mkdir -p $(CUR_DIR)/$(OBJ_PATH))

TARGET=cweb

$(OBJ_PATH)/$(TARGET) : $(OBJ)
	g++ $^ -o $@ $(link_flags)

$(OBJ_PATH)/%.o : %.cc
	g++ -c $< -o $@ $(compile_flags)
	
$(OBJ_PATH)/%.o : %.s
	g++ -c $< -o $@ $(compile_flags)
	
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
    

