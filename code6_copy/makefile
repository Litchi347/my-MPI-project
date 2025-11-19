# 编译器
CXX = mpicxx
CXXFLAGS = -O2 -Wall -std=c++11 -fopenmp
LDFLAGS = -fopenmp

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
	@echo "Linking targets..."
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# 编译每个 cpp -> o
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 调试目标
debug:CXXFLAGS = -g -O0 -Wall -std=c++11 -fopenmp
debug:clean all

# 清理
clean:
	@echo "Cleaning up ..."
	rm -f $(OBJS) $(TARGET)
	rm -f *.dat