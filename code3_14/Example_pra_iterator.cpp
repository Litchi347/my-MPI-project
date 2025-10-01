# include <vector>
# include <iostream>
# include <omp.h>

using namespace std;

void iterator_example(){
    vector<int> vec(23);
    cout << "vec.size():" <<vec.size() <<" threads:" << omp_get_max_threads() << endl;

    #pragma omp parallel for default(none) shared(vec)

    for(auto it = vec.begin();it < vec.end();it++){
        // do work with *it
        *it = omp_get_thread_num();
    }

    // for(int i = 0;i < vec.size();i++){
    //     // do work with *it
    //     vec[i] = omp_get_thread_num();
    // }

    for(int i = 0;i < vec.size();i++){
        cout << "vec[" << i << "] : "  << vec[i] << endl;
    }
}

int main(){
    iterator_example();
    return 0;
}