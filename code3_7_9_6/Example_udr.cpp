class V{
    float *p;
    int n;

    public:
    V(int _n):n(_n){
        p = new float[n];
    }
    V(const V&m):n(m.n){
        p = new float[n];
    }
    ~V(){
        delete[] p;
    }
    V& operator += (const V&);

    #pragma omp declare reduction(+:V:omp_out += omp_in) initializer(omp_priv(omp_orig))
};

# include <algorithm>
# include <list>
# include <vector>

using namespace std;

// 定义了如何对 vector 使用 + 进行归约
#pragma omp declare reduction(+:vector<int>: \
    transform(omp_out.begin(),omp_out.end(),\ 
    omp_in.begin(),omp_in.end(),plus<int>()))

// merge 用于连接两个 vector
#pragma omp declare reduction(merge:vector<int>: \
    omp_out.insert(omp_out.end(),omp_in.begin(),omp_in.end()))

// 为 list 定义一个 merge 操作
#pragma omp declare reduction(merge:list<int>: \
    omp_out.merge(omp_in))