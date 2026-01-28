#include <iostream> //converted to iostream
#include <vector>

using namespace std;


inline void sum(int& result, const vector<int>& data) { //used inline for function
    result = 0;
    for (const int& value : data) {
        result += value;
    }
}


int main()
{
    const int N = 40; //removed defines for constant
    vector <int> data(N); //replaced array with vector
    for(int i = 0; i< N; ++i){
    data [i] = i;
}

int accum;
sum (accum, data);
cout <<"Sum is " << accum << endl;
return 0;
}