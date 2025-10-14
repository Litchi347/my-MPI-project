CXX = mpiicpx                             # 定义变量CXX,表示要用到的C++编译器/命令
CXXFLAGS = -Wall -O2 -qopenmp             # 定义编译选项变量CXXFLAGS,-Wall打开常见警告,-O2表示优化等级

TARGET = program                          # 定义最终生成的可执行文件名

SRCS = $(wildcard *.cpp)                  # 使用make的wildcard函数获取当前目录下所有以.cpp结尾的源文件收集到变量SRCS中

OBJS = $(SRCS:.cpp=.o)                    # make的文本替换写法,把SRCS中的.cpp替换成.o,生成目标文件列表

# 生成最终可执行文件的规则:
# 左边$(TARGET)表示目标文件,右边$(OBJS)表示依赖的目标文件
# $@表示目标文件,$^表示所有依赖文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# 生成 .o 文件的规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理命令
clean:
	rm -f $(TARGET) $(OBJS)