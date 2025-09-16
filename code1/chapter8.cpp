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

	// 边界初始化
	if (myid == 0) {
		for (int i = 0; i < totalsize; i++) {
			a[idx(i, 1, mysize)] = 8.0;
		}
	}

	if (myid == numprocs-1) {
		for (int i = 0; i < totalsize; i++) {
			a[idx(i, mysize, mysize)] = 8.0;
		}
	}

	for (int j = 0; j < mysize + 2; j++) {
		a[idx(0, j, mysize)] = 8.0;
		a[idx(totalsize - 1, j, mysize)] = 8.0;
	}

	// 自定义列类型，用于通信
	MPI_Datatype coltype;
	MPI_Type_vector(totalsize, 1, mysize + 2, MPI_DOUBLE, &coltype);
	MPI_Type_commit(&coltype);

	int left = (myid >0) ? myid - 1 : MPI_PROC_NULL;
	int right = (myid < numprocs - 1) ? myid + 1 : MPI_PROC_NULL;

	MPI_Status status;

	for (int n = 0; n < steps; n++) {
		// 从左向右通信，按顺序发送接收，避免死锁

		// 偶数rank先发送右，接收左；奇数rank先接收左，发送右
		if(myid % 2 == 0){                                                                   // 偶数rank进程
			if(right != MPI_PROC_NULL){                                                      // 向右侧发送数据
				MPI_Send(&a[idx(0,mysize,mysize)],1,coltype,right,10,MPI_COMM_WORLD);
			}
			if(left != MPI_PROC_NULL){                                                       // 从左侧接收数据
				MPI_Recv(&a[idx(0,0,mysize)],1,coltype,left,10,MPI_COMM_WORLD,&status);
			}
		}else{                                                                                // 奇数rank进程
			if(left != MPI_PROC_NULL){                                                        // 从左侧接收数据
				MPI_Recv(&a[idx(0,0,mysize)],1,coltype,left,10,MPI_COMM_WORLD,&status);
			}
			if(right != MPI_PROC_NULL){                                                       // 向右侧发送数据
				MPI_Send(&a[idx(0,mysize,mysize)],1,coltype,right,10,MPI_COMM_WORLD);
			}
		}

		// 从右向左通信
		if(myid % 2 == 0){
			if(left != MPI_PROC_NULL){
				MPI_Send(&a[idx(0,1,mysize)],1,coltype,left,20,MPI_COMM_WORLD);
			}
			if(right != MPI_PROC_NULL){
				MPI_Recv(&a[idx(0,mysize+1,mysize)],1,coltype,right,20,MPI_COMM_WORLD,&status);
			}
		}else{
			if(right != MPI_PROC_NULL){
				MPI_Recv(&a[idx(0,mysize+1,mysize)],1,coltype,right,20,MPI_COMM_WORLD,&status);
			}
			if(left != MPI_PROC_NULL){
				MPI_Send(&a[idx(0,1,mysize)],1,coltype,left,20,MPI_COMM_WORLD);
			}
		}
	}

	// Jacobi迭代
	int begin_col = (myid == 0 ? 2 : 1);
	int end_col = (myid == numprocs - 1 ? mysize - 1 : mysize);

	for(int j=begin_col;j<=end_col;j++){
		for(int i=1;i<totalsize-1;i++){
			b[idx(i,j,mysize)] = 0.25 *(a[idx(i,j+1,mysize)] + a[idx(i,j-1,mysize)] + a[idx(i+1,j,mysize)] + a[idx(i-1,j,mysize)]);
		}
	}

	for(int j=begin_col;j<=end_col;j++){
		for(int i=1;i<totalsize-1;i++){
			a[idx(i,j,mysize)] = b[idx(i,j,mysize)];
		}
	}

	// 输出结果
	for(int i=1;i<totalsize-1;i++){
		cout << "Process " << myid << ":";
		for(int j=begin_col;j<= end_col;j++){
			cout << a[idx(i,j,mysize)] << " ";
		}
		cout << endl;
	}

	MPI_Type_free(&coltype);
	MPI_Finalize();
	return 0;
}
