import threading
import time
import random


customer_ready = threading.Semaphore(0) 
barber_ready = threading.Semaphore(0)
seats_mutex = threading.Lock()

WAITING_ROOM_CAPACITY = 3
waiting_seats = WAITING_ROOM_CAPACITY
TOTAL_CUSTOMERS = 10
shop_is_open = True

def barber():
    global shop_is_open
    print(" [Barber] Shop is open. Waiting for customers...")
    
    while shop_is_open or waiting_seats < WAITING_ROOM_CAPACITY:
        acquired = customer_ready.acquire(timeout=1) 
        
        if not acquired:
            if not shop_is_open and waiting_seats == WAITING_ROOM_CAPACITY:
                break
            continue

        barber_ready.release()
        
        print("  [Barber] Cutting hair...")
        time.sleep(1.5) 
        print(" [Barber] Haircut finished.")

    print(" [Barber] All done. Going home.")

def customer(index):
    global waiting_seats
    
    with seats_mutex:
        if waiting_seats > 0:
            waiting_seats -= 1
            print(f" [Customer {index}] Entered waiting room. Seats left: {waiting_seats}")

            customer_ready.release()
        else:
            print(f" [Customer {index}] Waiting room full. Leaving...")
            return
        
    barber_ready.acquire()
    
    with seats_mutex:
        waiting_seats += 1

if __name__ == "__main__":
    print("--- Simulation Started: Sleeping Barber Problem ---")
    
    barber_thread = threading.Thread(target=barber)
    barber_thread.start()

    customer_threads = []
    for i in range(TOTAL_CUSTOMERS):
        t = threading.Thread(target=customer, args=(i+1,))
        customer_threads.append(t)
        t.start()
        time.sleep(random.uniform(0.5, 1.5))

    for t in customer_threads:
        t.join()

    print(" [Main] No more customers. Closing shop.")
    shop_is_open = False
    barber_thread.join()

    print("--- Simulation Complete. ---")