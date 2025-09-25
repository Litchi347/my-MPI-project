# include <mpi.h>
# include <iostream>
# include <vector>
# include <iomanip>

using namespace std;

inline int idx(int i, int j, int mysize) {                                          // 分配连续内存
	return i * (mysize + 2) + j; 
}

int main(int argc, char* argv[]) {
	const int totalsize = 10000;                                                    // 增大计算量来体现不同进程数计算时的性能差异
	const int steps = 10;

	MPI_Init(&argc, &argv);

	int myid = -1, numprocs = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	if (numprocs <= 0) {
		if (myid == 0) {
			cerr << "Invalid numpprocs\n";
		}
		MPI_Finalize();
		return 1;
	}

	if (totalsize % numprocs != 0) {
		if (myid == 0) {
			cerr << "Error: totalsize must be divisible by numprocs\n";
		}
		MPI_Finalize();
		return 1;
	}
	const int mysize = totalsize / numprocs;

    // 输出会影响计算运行时间
	// if (myid == 0) {
	// 	cout << "Running with totalsize=" << totalsize
	// 		<< " numprocs=" << numprocs
	// 		<< " (mysize=" << mysize << "),step=" << steps << endl;
	// }
	// cout << "Process " << myid << " of " << numprocs << " is alive" << endl;

	vector<double> a(totalsize * (mysize + 2), 0.0);
	vector<double> b(totalsize * (mysize + 2), 0.0);

	if (myid == 0) {
		for (int i = 0; i < totalsize; i++) {
			a[idx(i, 1, mysize)] = 1.0;
		}
	}

	if (myid == numprocs - 1) {
		for (int i = 0; i < totalsize; i++) {
			a[idx(i, mysize, mysize)] = 1.0;
		}
	}

	for (int j = 0; j < mysize + 2; j++) {
		a[idx(0, j, mysize)] = 8.0;
		a[idx(totalsize - 1, j, mysize)] = 1.0;
	}

    // 记录运行时间，并且实现同步
    double t_start,t_end;
    MPI_Barrier(MPI_COMM_WORLD);
    t_start = MPI_Wtime();

	// 创建一个MPI列类型，将其他进程的计算结果打包进行进程间通信
	MPI_Datatype coltype;
	MPI_Type_vector(totalsize, 1, mysize + 2, MPI_DOUBLE, &coltype);
	MPI_Type_commit(&coltype);

	// 左右邻居进程标识，使用MPI_PROC_NULL便于边界处理
	int left = (myid > 0) ? myid - 1 : MPI_PROC_NULL;
	int right = (myid < numprocs - 1) ? myid + 1 : MPI_PROC_NULL;

	const int tag1 = 3;
	const int tag2 = 4;
	MPI_Status status;

	for (int n = 0; n < steps; n++) {
		// 从左向右平移数据
		MPI_Sendrecv(
			&a[idx(0, mysize, mysize)], 1, coltype, right, tag1,
			&a[idx(0, 0, mysize)], 1, coltype, left, tag1, 
			MPI_COMM_WORLD, &status);

		// 从右向左平移数据
		MPI_Sendrecv(
			&a[idx(0, 1, mysize)], 1, coltype, left, tag2,
			&a[idx(0, mysize + 1, mysize)], 1, coltype, right, tag2, 
			MPI_COMM_WORLD, &status);

		int begin_col = 1;
		int end_col = mysize;
		if (myid == 0) {
			begin_col = 2;
		}
		if (myid == numprocs - 1) {
			end_col = mysize - 1;
		}

		for (int j = begin_col; j <= end_col; j++) {
			for (int i = 1; i < totalsize - 1; i++) {
				b[idx(i, j, mysize)] =
					0.25 * (a[idx(i, j + 1, mysize)]
						+ a[idx(i, j - 1, mysize)]
						+ a[idx(i + 1, j, mysize)]
						+ a[idx(i - 1, j, mysize)]);
			}
		}
		for (int j = begin_col; j <= end_col; ++j) {
			for (int i = 1; i < totalsize - 1; ++i) {
				a[idx(i, j, mysize)] = b[idx(i, j, mysize)];
			}
		}
	}

    MPI_Barrier(MPI_COMM_WORLD);
    t_end = MPI_Wtime();
    double local_time = t_end - t_start;
    double max_time;
    MPI_Reduce(&local_time,&max_time,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);


	// int out_begin = (myid = 0 ? 2 : 1);
	// int out_end = (myid == numprocs - 1 ? mysize - 1 : mysize);

	// for (int i = 1; i < totalsize - 1; i++) {
	// 	cout << "Process " << myid << ": ";
	// 	for (int j = out_begin; j <= out_end; j ++ ) {
	// 		cout << a[idx(i, j, mysize)] << " ";
	// 	}
	// 	cout << endl;
	// }

    if(myid == 0){
        cout << "Time of calculation=" << fixed << setprecision(6) << max_time << " s" << endl;
    }

	MPI_Type_free(&coltype);
	MPI_Finalize();

	return 0;
}