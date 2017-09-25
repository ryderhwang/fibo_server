#include <rfpgt_threadpool.h>

#include <chrono>
#include <iostream>
#include <tuple>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{
    cout << "Hello world!" << endl;

    ThreadPool                       pool(4);
    vector<dplp::Promise<int> > results;

    for (int i = 0; i < 8; i++) {
        auto result = (pool.enqueue([i] {
            cout << "hello " << i << endl;
            this_thread::sleep_for(chrono::seconds(1));
            cout << "world " << i << endl;
            return i * i;
        }));
        result.then([](int i) { cout << i; });
        results.emplace_back(move(result));
    }

    std::cout << std::endl;
    return 0;
}
