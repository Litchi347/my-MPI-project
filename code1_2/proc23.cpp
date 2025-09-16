# include <mpi.h>
# include <iostream>
# include <vector>

using namespace std;

inline int idx(int i, int j, int mysize) {
	return i * (mysize + 2) + j;
}

int main(int argc, char* argv[]) {
	const int totalsize = 16;
	const int steps = 10;

	MPI_Init(&argc, &argv);

	int myid, numprocs;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    const int mysize = totalsize / numprocs;

	if (totalsize % numprocs != 0) {
		if (myid == 0) {
			cerr << "Error: totalsize must be divisible by numprocs\n";
		}
		MPI_Finalize();
		return 1;
	}

	cout << "Process " << myid << " of " << numprocs << " is alive" << endl;

	vector<double> a(totalsize * (mysize + 2), 0.0);
	vector<double> b(totalsize * (mysize + 2), 0.0);

    // 初始化边界值
	if (myid == 0) {
		for (int i = 0; i < totalsize; i++) {
			a[idx(i, 1, mysize)] = 8.0;
		}
	}

	if (myid == numprocs - 1) {
		for (int i = 0; i < totalsize; i++) {
			a[idx(i, mysize, mysize)] = 8.0;
		}
	}

	for (int j = 0; j < mysize + 2; j++) {
		a[idx(0, j, mysize)] = 8.0;
		a[idx(totalsize - 1, j, mysize)] = 8.0;
	}

    MPI_Datatype coltype;
	MPI_Type_vector(totalsize, 1, mysize + 2, MPI_DOUBLE, &coltype);
	MPI_Type_commit(&coltype);

	// 左右邻居进程标识，使用MPI_PROC_NULL便于边界处理
    // MPI_PROC_NULL: 虚拟进程
	int left = (myid > 0) ? myid - 1 : MPI_PROC_NULL;
	int right = (myid < numprocs - 1) ? myid + 1 : MPI_PROC_NULL;

	MPI_Status status;

	for (int n = 0; n < steps; n++) {

		// 从左向右平移数据
		MPI_Sendrecv(
			&a[idx(0, mysize, mysize)], 1, coltype, right, 3,
			&a[idx(0, 0, mysize)], 1, coltype, left, 3,
			MPI_COMM_WORLD, &status);

		// 从右向左平移数据
		MPI_Sendrecv(
			&a[idx(0, 1, mysize)], 1, coltype, left, 4,
			&a[idx(0, mysize + 1, mysize)], 1, coltype, right, 4,
			MPI_COMM_WORLD, &status);

	    int begin_col = (myid = 0 ? 2 : 1);
	    int end_col = (myid == numprocs - 1 ? mysize - 1 : mysize);

		for (int j = begin_col; j <= end_col; j++) {
			for (int i = 1; i < totalsize - 1; i++) {
				b[idx(i, j, mysize)] = 0.25 * (a[idx(i, j + 1, mysize)] + a[idx(i, j - 1, mysize)] + a[idx(i + 1, j, mysize)] + a[idx(i - 1, j, mysize)]);
			}
		}

		for (int j = begin_col; j <= end_col; j++) {
			for (int i = 1; i < totalsize - 1; i++) {
				a[idx(i, j, mysize)] = b[idx(i, j, mysize)];
			}
		}
	}

	int out_begin = (myid = 0 ? 2 : 1);
	int out_end = (myid == numprocs - 1 ? mysize - 1 : mysize);

	for (int i = 1; i < totalsize - 1; i++) {
		cout << "Process " << myid << ": ";
		for (int j = out_begin; j <= out_end; j ++ ) {
			cout << a[idx(i, j, mysize)] << " ";
		}
		cout << endl;
	}
	MPI_Type_free(&coltype);
	MPI_Finalize();

	return 0;
}