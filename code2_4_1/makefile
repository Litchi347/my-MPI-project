# 默认开启调试模式(DEBUG=1)
DEBUG ?= 1
PROFILE ?= 0
VTUNE ?= 0                                # 是否启用 vTune 分析

# 设置 Vtune 结果保存目录
RESULT_DIR := ./vtune_result

# 编译器设置
CXX = mpiicpx                             # MPI C++编译器

# 模式切换逻辑

ifeq ($(DEBUG),1)
	# 性能剖析模式
	CXXFLAGS = -Wall -pg -g -O2
	MODE = Profile_Mode_pg
else ifeq ($(DEBUG),1)
	# 调试模式
	CXXFLAGS = -Wall -g -O0
	MODE = DEBUG_Mode_g_O0
else
	# 发布模式
	CXXFLAGS = -wall -O2
	MODE = Release_Mode_O2
endif

# 目标与文件设置
TARGET = program                          # 最终生成的可执行文件名
SRCS = $(wildcard *.cpp)                  # 自动收集所有的 .cpp 文件
OBJS = $(SRCS:.cpp=.o)                    # 对应的 .o 文件列表

# 生成可执行文件
all: $(TARGET)

# 链接生成可执行文件
$(TARGET): $(OBJS)
	@echo "Building $(TARGET) in $(MODE)"
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# 编译 .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理命令
clean:
	rm -f $(TARGET) $(OBJS) gmon.out gprof_report.txt profile.txt profile.dot gprof.svg
	rm -rf vtune_result*
	@echo "clean complete."

# gprod 分析
profile:clean $(TARGET)
	@echo ">>> Running program to generate gmon.out ..."
	mpirun -np 4 ./$(TARGET)
	@echo ">>> generating gprof report ..."
	gprof -b ./$(TARGET) gmon.out > gprof_report.txt
	@echo ">>> Profiling complete! See gprof_report.txt"

# vTune 分析
vtune:clean $(TARGET)
	@echo ">>> Running vTune analysis ..."
	# 执行 vTune Hotspots 收集
	mpirun -np 4 vtune -collect hotspots -result-dir $(RESULT_DIR) -- ./$(TARGET)
	@echo ">>> vTune results saved in $(RESULT_DIR)"