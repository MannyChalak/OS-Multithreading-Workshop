#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(500, 1000); // milliseconds

class Philosopher {
public:
    Philosopher(int idx, std::mutex &left, std::mutex &right, int meals = 3)
        : index(idx), left_fork(left), right_fork(right), meals_to_eat(meals) {}

    void operator()() {
        for (int i = 0; i < meals_to_eat; ++i) {
            std::cout << " [Philosopher " << index << "] is Thinking..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds((int)dis(gen)));

            std::cout << " [Philosopher " << index << "] is Hungry." << std::endl;

            // Deadlock prevention: pick lower-numbered fork first
            std::mutex &first_fork = (index < (index + 1) % 5) ? left_fork : right_fork;
            std::mutex &second_fork = (&first_fork == &left_fork) ? right_fork : left_fork;

            std::lock(first_fork, second_fork);
            std::lock_guard<std::mutex> lock1(first_fork, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(second_fork, std::adopt_lock);

            std::cout << " [Philosopher " << index << "] is EATING (Meal " << i+1 << "/" << meals_to_eat << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds((int)dis(gen)));

            std::cout << "  [Philosopher " << index << "] put down forks." << std::endl;
        }
        std::cout << " [Philosopher " << index << "] is full and left the table." << std::endl;
    }

private:
    int index;
    std::mutex &left_fork;
    std::mutex &right_fork;
    int meals_to_eat;
};

int main() {
    std::cout << "--- Simulation Started: Dining Philosophers Problem ---" << std::endl;

    std::vector<std::mutex> forks(5);
    std::vector<std::thread> philosophers;

    for (int i = 0; i < 5; ++i) {
        philosophers.push_back(std::thread(Philosopher(i, forks[i], forks[(i+1)%5])));
    }

    for (auto &p : philosophers) {
        p.join();
    }

    std::cout << "--- All philosophers finished eating. Simulation Complete. ---" << std::endl;
    return 0;
}