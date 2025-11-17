# 默认开启调试模式(DEBUG=1)
DEBUG ?= 1                                # 是否开启调试模式(-g -O0)
PROFILE ?= 0                              # 是否启用 gprof 性能剖析(-pg)
VTUNE ?= 0                                # 是否启用 vTune 分析(自动添加 -g -O2)

# Vtune 结果保存目录
RESULT_DIR ?= vtune_result_P4_Hotspots

# 运行进程数(用于 vtune 和 valgring 目标，默认 P = 4)
NP ?= 4

# Jacobi 矩阵大小参数(默认 5000 5000)
ARGS ?= 5000 5000

# 编译器设置
CXX = mpiicpx                             # Intel MPI C++编译器

# 模式切换逻辑

# 优先检查性能剖析和 vtune 模式
ifeq ($(PROFILE),1)
	CXXFLAGS = -Wall -pg -g -O2
	MODE = Profile_Mode_pg
else ifeq ($(Vtune),1)
	# vtune 需要 -g 来显示源代码行，通常配合 -O2 以分析优化后的代码性能
	CXXFLAGS = -Wall -g -O2
	MODE = VTUNE_Mode_g_O2
else ifeq ($(DEBUG),1)
	# 调试模式
	CXXFLAGS = -Wall -g -O0
	MODE = DEBUG_Mode_g_O0
else
	# 发布模式(默认)
	CXXFLAGS = -Wall -O2
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
	@echo "==================================================================="
	@echo "Building $(TARGET) in $(MODE)"
	@echo "==================================================================="
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# 编译 .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY:clean profile valgring vtune vtune_scaling
# 内存泄露分析目标（使用 Valgrind)
# 默认使用单进程 P = 1 进行分析，以隔离 MPI 库的内部泄露
# 使用方法：make valgrind
valgrind:clean
	@echo ">>> Compiling target for Valgrind (P=1 mode)..."
	$(MAKE) $(TARGET) VTUNE=1             # 使用 VTUNE=1 确保 -g -O2 编译
	@echo ">>> Running Valgrind Memcheck on P=1. Output in valgrind_report.txt"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --log-file=valgrind_report.txt ./$(TARGET) $(ARGS)
	@echo ">>> Valgrind complete.Check valgrind_report.txt for definitely lost bytes."

# gprof 性能剖析目标（关注热点函数和算子）
# 使用方法：make profile
profile:clean
	@echo ">>> Compiling target for gprof (-pg)..."
	$(MAKE) $(TARGET) PROFILE=1
	@echo ">>> Running program to generate gmon.out (P=$(NP))..."
	mpirun -np $(NP) ./$(TARGET) $(ARGS)
	@echo ">>> generating gprof report..."
	gprof -b ./$(TARGET) gmon.out > gprof_report.txt
	@echo ">>> Profiling complete! See gprof_report.txt"

# vTune Hotspots 分析目标(关注热点函数/算子和通信占比)
# 默认运行 P = 4，可通过 NP 参数修改
# 使用方法： make vtune NP=8 ARGS="5000 5000" 
vtune:clean
	@echo ">>> Compiling target for vTune (-g -O2)..."
	$(MAKE) $(TARGET) VTUNE=1
	@echo ">>> Running vTune Hotspots analysis on P=$(NP).Result in $(RESULT_DIR)_P$(NP)"
	mpirun -np $(NP) vtune -collect hotspots -result-dir $(RESULT_DIR) -- ./$(TARGET) $(ARGS)
	@echo ">>> vTune analysis complete."

# vTune 扩展性分析目标（批量运行，用于构建扩展性表）
# 使用方法： make vtune_scaling
vtune_scaling:
	@echo ">>> Compiling target for vTune Scaling (-g -O2)..."
	$(MAKE) $(TARGET) VTUNE=1
	@echo ">>> Running P=4,8,16,20 Hotspots tests for scaling analysis..."
	# P=4 Hotspots
	mpirun -np 4 vtune -collect hotspots -result-dir vtune_scaling_P4 -- ./$(TARGET) 4960 4960
	# P=8 Hotspots
	mpirun -np 8 vtune -collect hotspots -result-dir vtune_scaling_P8 -- ./$(TARGET) 4960 4960
	# P=16 Hotspots
	mpirun -np 16 vtune -collect hotspots -result-dir vtune_scaling_P16 -- ./$(TARGET) 4960 4960
	# P=20 Hotspots
	mpirun -np 20 vtune -collect hotspots -result-dir vtune_scaling_P20 -- ./$(TARGET) 4960 4960
	@echo ">>> Scaling tests complete.Results in vtune_scaling_P*"

# 清理命令
clean:
	rm -f $(TARGET) $(OBJS) gmon.out program_gprof
	rm -rf vtune_result* vtune_scaling_* valgrind_report_*
	@echo "clean complete."