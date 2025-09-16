# 使用的编译器
CXX = g++
# 编译参数，-Wall 开所有警告，-O2 优化
CXXFLAGS = -Wall -O2

# 自动找到当前目录下所有 .cpp 文件
SRCS = $(wildcard *.cpp)
# 把 .cpp 改成 .o 目标文件
OBJS = $(SRCS:.cpp=.o)

# 最终生成的可执行文件名（自己改成喜欢的名字）
TARGET = program

# 默认目标：生成可执行文件
all: $(TARGET)

# 链接规则
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译规则：把 .cpp 编译成 .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则
clean:
	rm -f $(OBJS) $(TARGET)
