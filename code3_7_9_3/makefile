# 使用 Intel oneAPI 编译器的 MPI wrapper
CC      = mpiicx                        # C 语言编译器
CXX     = mpiicpx                       # C++ 编译器

CFLAGS   = -Wall -O2 -qopenmp -fiopenmp # C 编译选项
CXXFLAGS = -Wall -O2 -qopenmp -fiopenmp # C++ 编译选项

TARGET_FLAGS = -fopenmp-targets=spir64

TARGET  = program                       # 生成的可执行文件名

# 自动搜集源文件
CSRCS   = $(wildcard *.c)               # 所有 .c 文件
CPPSRCS = $(wildcard *.cpp)             # 所有 .cpp 文件

# 生成对应的目标文件列表
COBJS   = $(CSRCS:.c=.o)
CPPOBJS = $(CPPSRCS:.cpp=.o)
OBJS    = $(COBJS) $(CPPOBJS)

# 链接规则：统一用 C++ 编译器链接（兼容性更好）
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(TARGET_FLAGS) -o $@ $(OBJS)

# C 文件的编译规则
%.o: %.c
	$(CC) $(CFLAGS) $(TARGET_FLAGS) -c $< -o $@

# C++ 文件的编译规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(TARGET_FLAGS) -c $< -o $@

# 清理命令
clean:
	rm -f $(TARGET) $(OBJS)
