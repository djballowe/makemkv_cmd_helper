#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

void loadingSpinner(std::atomic<bool> &finished) {
    const std::vector<char> spinner = {'|', '/', '-', '\\'};
    int i = 0;
    while (!finished) {
        std::cout << "\r" << spinner[i] << " " << "Analyzing disc and assigning tracks " << std::flush;
        i = (i + 1) % spinner.size();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
