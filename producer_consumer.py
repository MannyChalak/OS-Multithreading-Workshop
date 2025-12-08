import threading
import time
import random


buffer = []
BUFFER_CAPACITY = 5
TOTAL_ITEMS_TO_PRODUCE = 10

condition = threading.Condition()

def producer():
    global buffer
    for i in range(TOTAL_ITEMS_TO_PRODUCE):
        with condition:
            while len(buffer) == BUFFER_CAPACITY:
                print(f"  [Producer] Buffer is full {buffer}. Waiting...")
                condition.wait()
            
            item = random.randint(1, 100)
            buffer.append(item)
            print(f" [Producer] Produced: {item} | Buffer: {buffer}")
            condition.notify()
        time.sleep(random.uniform(0.5, 1.0))
    
    print(" [Producer] Finished producing items.")

def consumer():
    global buffer
    for i in range(TOTAL_ITEMS_TO_PRODUCE):
        with condition:
            while len(buffer) == 0:
                print(" [Consumer] Buffer is empty. Waiting...")
                condition.wait()
                
            item = buffer.pop(0)
            print(f" [Consumer] Consumed: {item} | Buffer: {buffer}")
            condition.notify()
        
        time.sleep(random.uniform(0.8, 1.2))

    print(" [Consumer] Finished consuming items.")


if __name__ == "__main__":
    print("--- Simulation Started: Producer-Consumer Problem ---")
    
    t1 = threading.Thread(target=producer)
    t2 = threading.Thread(target=consumer)

    t1.start()
    t2.start()

    t1.join()
    t2.join()

    print("--- All threads finished. Simulation Complete. ---")