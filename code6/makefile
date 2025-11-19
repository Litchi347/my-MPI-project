# 编译器
CXX = g++
CXXFLAGS = -O2 -Wall -std=c++11

# 自动查找当前目录下所有.cpp文件
SRCS = $(wildcard *.cpp)

# 把 .cpp 改成 .o
OBJS = $(SRCS:.cpp=.o)

# 可执行文件名
TARGET = main.out

# 默认目标
all:$(TARGET)

# 链接
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译每个 cpp -> o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -f *.o $(TARGET)