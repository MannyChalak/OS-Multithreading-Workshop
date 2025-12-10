#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>

int shared_data = 0;
std::mutex resource_lock;
std::mutex reader_count_lock;
int reader_count = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis_read(500, 2000); // milliseconds for readers
std::uniform_int_distribution<> dis_write(1000, 3000); // milliseconds for writers

void reader(int reader_id) {
    for (int i = 0; i < 3; ++i) {
        // ENTRY SECTION
        {
            std::lock_guard<std::mutex> lock(reader_count_lock);
            reader_count++;
            if (reader_count == 1)
                resource_lock.lock();
        }

        std::cout << "[Reader-" << reader_id << "] Reading data: " << shared_data
                  << " | Active Readers: " << reader_count << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // EXIT SECTION
        {
            std::lock_guard<std::mutex> lock(reader_count_lock);
            reader_count--;
            if (reader_count == 0)
                resource_lock.unlock();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(dis_read(gen)));
    }
    std::cout << "✅ [Reader-" << reader_id << "] Finished execution." << std::endl;
}

void writer(int writer_id) {
    for (int i = 0; i < 2; ++i) {
        resource_lock.lock();
        std::cout << "  [Writer-" << writer_id << "] WRITING DATA... (Lock held)" << std::endl;
        shared_data += 1;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << " [Writer-" << writer_id << "] Updated data to: " << shared_data << std::endl;
        resource_lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(dis_write(gen)));
    }
    std::cout << "✅ [Writer-" << writer_id << "] Finished execution." << std::endl;
}

int main() {
    std::cout << "--- Simulation Started: Readers-Writers Problem ---" << std::endl;

    std::vector<std::thread> threads;

    // ایجاد 3 خواننده
    for (int i = 1; i <= 3; ++i) {
        threads.push_back(std::thread(reader, i));
    }

    // ایجاد 1 نویسنده
    threads.push_back(std::thread(writer, 1));

    for (auto &t : threads) {
        t.join();
    }

    std::cout << "--- All threads finished. Simulation Complete. ---" << std::endl;
    return 0;
}