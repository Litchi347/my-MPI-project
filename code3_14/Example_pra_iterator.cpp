# include <vector>
# include <iostream>
using namespace std;

void iterator_example(){
    vector<int> vec(23);
    vector<int>::iterator it;

    #pragma omp parallel for default(none) shared(vec)
    // 编译通过，但是运行不出结果
    for(it = vec.begin();it < vec.end();it++){
        *it = 42;
        // do work with *it
    }

    // for(int i=0;i<vec.size();i++){
    //     vec[i] = 42;
    // }

    for(int i=0;i<vec.size();i++){
        cout << "vec[i]: " << vec[i] << endl;
    }
}

int main(){
    iterator_example();
    return 0;
}