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

	int myid, numprocs;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	const int mysize = totalsize / numprocs;

	cout << "Process " << myid << " of " << numprocs << " is alive" << endl;

	vector<double> a(totalsize * (mysize + 2), 0.0);
	vector<double> b(totalsize * (mysize + 2), 0.0);

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

	MPI_Status status;

	for (int i = 0; i < steps; i++) {

		// 从左向右通信
		// 最左边的进程，需要向右边的进程发送数据
		if (myid == 0) {
			MPI_Send(&a[idx(0, mysize, mysize)], 1, coltype,
				myid + 1, 10, MPI_COMM_WORLD);
		}
		// 最右边的进程，需要从左边的进程接收数据
		else if (myid == numprocs - 1) {
			MPI_Recv(&a[idx(0, 0, mysize)], 1, coltype,
				myid - 1, 10, MPI_COMM_WORLD, &status);
		}
		else {
			MPI_Sendrecv(&a[idx(0, mysize, mysize)], 1, coltype,    // 将右幽灵列的数据发送给右边的进程
				myid + 1, 10,
				&a[idx(0, 0, mysize)], 1, coltype,                  // 将左幽灵列的数据发送给左边的进程
				myid - 1, 10,
				MPI_COMM_WORLD, &status);
		}

		// 从右向左通信
		if (myid == 0) {
			MPI_Recv(&a[idx(0, mysize + 1, mysize)], 1, coltype,
				myid + 1, 10, MPI_COMM_WORLD, &status);
		}
		else if (myid == numprocs - 1) {
			MPI_Send(&a[idx(0, 1, mysize)], 1, coltype,
				myid - 1, 10, MPI_COMM_WORLD);
		}
		else {
			MPI_Sendrecv(&a[idx(0, 1, mysize)], 1, coltype,
				myid - 1, 10,
				&a[idx(0, mysize + 1, mysize)], 1, coltype,
				myid + 1, 10,
				MPI_COMM_WORLD, &status);
		}

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
				b[idx(i, j, mysize)] = 0.25 * (
					a[idx(i, j + 1, mysize)] + a[idx(i, j - 1, mysize)] +
					a[idx(i + 1, j, mysize)] + a[idx(i - 1, j, mysize)]);
			}
		}

		for (int j = begin_col; j <= end_col; j++) {
			for (int i = 1; i < totalsize - 1; i++) {
				a[idx(i, j, mysize)] = b[idx(i, j, mysize)];
			}
		}
	}

	int begin_col = (myid == 0 ? 2 : 1);
	int end_col = (myid == numprocs - 1 ? mysize - 1 : mysize);

	for (int i = 1; i < totalsize - 1; i++) {
		cout << "Process " << myid << ": ";
		for (int j = begin_col; j <= end_col; j++) {
			cout << a[idx(i, j, mysize)] << " ";
		}
		cout << endl;
	}

	MPI_Finalize();
	return 0;
}