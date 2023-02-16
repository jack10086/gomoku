CXX = g++
TARGET = gomoku 
# 依赖文件  .o
SRC = $(wildcard *.cpp)
OBJ = $(patsubst *.cpp, %.o, $(SRC))
CXXFLAGS = -c -Wall
LIBS:=-leasyx -lwinmm

$(TARGET) :$(OBJ)
	$(CXX) $^ -o $@ $(LIBS) 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
.PHONY:clean
clean:
	$(RM) $(TARGET) $(OBJS)
