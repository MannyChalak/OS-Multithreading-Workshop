#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <random>

std::vector<int> buffer;
const int BUFFER_CAPACITY = 5;
const int TOTAL_ITEMS_TO_PRODUCE = 10;

std::mutex mtx;
std::condition_variable cv;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis_prod(1, 100);
std::uniform_real_distribution<> dis_sleep_prod(500, 1000);  // ms
std::uniform_real_distribution<> dis_sleep_cons(800, 1200);  // ms

void producer() {
    for (int i = 0; i < TOTAL_ITEMS_TO_PRODUCE; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [](){ return buffer.size() < BUFFER_CAPACITY; });

        int item = dis_prod(gen);
        buffer.push_back(item);
        std::cout << " [Producer] Produced: " << item << " | Buffer: [";
        for (auto &b : buffer) std::cout << b << " ";
        std::cout << "]" << std::endl;

        cv.notify_all();
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds((int)dis_sleep_prod(gen)));
    }
    std::cout << " [Producer] Finished producing items." << std::endl;
}

void consumer() {
    for (int i = 0; i < TOTAL_ITEMS_TO_PRODUCE; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [](){ return !buffer.empty(); });

        int item = buffer.front();
        buffer.erase(buffer.begin());
        std::cout << " [Consumer] Consumed: " << item << " | Buffer: [";
        for (auto &b : buffer) std::cout << b << " ";
        std::cout << "]" << std::endl;

        cv.notify_all();
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds((int)dis_sleep_cons(gen)));
    }
    std::cout << " [Consumer] Finished consuming items." << std::endl;
}

int main() {
    std::cout << "--- Simulation Started: Producer-Consumer Problem ---" << std::endl;

    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    std::cout << "--- All threads finished. Simulation Complete. ---" << std::endl;
    return 0;
}