#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <random>

const int WAITING_ROOM_CAPACITY = 3;
const int TOTAL_CUSTOMERS = 10;

int waiting_seats = WAITING_ROOM_CAPACITY;
bool shop_is_open = true;

std::mutex mtx;
std::condition_variable cv_customer; // Customers waiting
std::condition_variable cv_barber;   // Barber ready
bool barber_is_ready = false;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis_ms(500, 1500); // ms for customer arrival

void barber() {
    std::cout << " [Barber] Shop is open. Waiting for customers..." << std::endl;

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until there is a customer or shop is closed
        cv_customer.wait(lock, []() { return waiting_seats < WAITING_ROOM_CAPACITY || !shop_is_open; });

        // If no customers and shop closed, exit
        if (waiting_seats == WAITING_ROOM_CAPACITY && !shop_is_open)
            break;

        // Only serve if there is a customer
        if (waiting_seats < WAITING_ROOM_CAPACITY) {
            barber_is_ready = true;
            cv_barber.notify_one();

            lock.unlock();
            std::cout << "  [Barber] Cutting hair..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            std::cout << " [Barber] Haircut finished." << std::endl;
        }
    }

    std::cout << " [Barber] All done. Going home." << std::endl;
}

void customer(int index) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (waiting_seats > 0) {
            waiting_seats--;
            std::cout << " [Customer " << index << "] Entered waiting room. Seats left: "
                      << waiting_seats << std::endl;
            cv_customer.notify_one();
        } else {
            std::cout << " [Customer " << index << "] Waiting room full. Leaving..." << std::endl;
            return;
        }
    }

    // Wait until barber is ready
    std::unique_lock<std::mutex> lock(mtx);
    cv_barber.wait(lock, []() { return barber_is_ready; });
    barber_is_ready = false;

    // Customer got haircut, free a seat
    waiting_seats++;
}

int main() {
    std::cout << "--- Simulation Started: Sleeping Barber Problem ---" << std::endl;

    std::thread barber_thread(barber);

    std::vector<std::thread> customer_threads;
    for (int i = 1; i <= TOTAL_CUSTOMERS; ++i) {
        customer_threads.emplace_back(customer, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis_ms(gen)));
    }

    // Wait for all customers to finish
    for (auto &t : customer_threads)
        t.join();

    // Close the shop and wake barber
    {
        std::lock_guard<std::mutex> lock(mtx);
        shop_is_open = false;
    }
    cv_customer.notify_one();

    barber_thread.join();

    std::cout << "--- Simulation Complete. ---" << std::endl;
    return 0;
}