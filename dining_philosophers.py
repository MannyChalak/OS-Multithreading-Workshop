import threading
import time
import random


class Philosopher(threading.Thread):
    def __init__(self, index, left_fork, right_fork, meals_to_eat=3):
        threading.Thread.__init__(self)
        self.index = index
        self.left_fork = left_fork
        self.right_fork = right_fork
        self.meals_to_eat = meals_to_eat

    def run(self):
        for i in range(self.meals_to_eat):
            print(f" [Philosopher {self.index}] is Thinking...")
            time.sleep(random.uniform(0.5, 1))
            
            print(f" [Philosopher {self.index}] is Hungry.")
            
            # --- EATING (DEADLOCK PREVENTION) ---
            # Resource Hierarchy Solution:
            # Always pick up the lower-numbered fork first.
            # This ensures no circular wait can occur.
            
            first_fork = self.left_fork if self.index < (self.index + 1) % 5 else self.right_fork
            second_fork = self.right_fork if first_fork == self.left_fork else self.left_fork

            with first_fork:
                with second_fork:
                    print(f" [Philosopher {self.index}] is EATING (Meal {i+1}/{self.meals_to_eat})")
                    time.sleep(random.uniform(0.5, 1))
            
            print(f"  [Philosopher {self.index}] put down forks.")
        
        print(f" [Philosopher {self.index}] is full and left the table.")

if __name__ == "__main__":
    print("--- Simulation Started: Dining Philosophers Problem ---")
    
    forks = [threading.Lock() for _ in range(5)]
    philosophers = [Philosopher(i, forks[i], forks[(i+1)%5]) for i in range(5)]

    for p in philosophers:
        p.start()

    for p in philosophers:
        p.join()

    print("--- All philosophers finished eating. Simulation Complete. ---")