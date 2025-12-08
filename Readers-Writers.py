import threading
import time
import random

shared_data = 0

resource_lock = threading.Lock()
reader_count_lock = threading.Lock()
reader_count = 0

def reader(reader_id):
    global reader_count
    
    for i in range(3):
        with reader_count_lock:
            reader_count += 1
            if reader_count == 1:
                resource_lock.acquire()
        
        print(f"[Reader-{reader_id}] Reading data: {shared_data} | Active Readers: {reader_count}")
        time.sleep(1)
        
        # --- EXIT SECTION ---
        with reader_count_lock:
            reader_count -= 1
            if reader_count == 0:
                resource_lock.release()
        
        time.sleep(random.uniform(0.5, 2))

    print(f"✅ [Reader-{reader_id}] Finished execution.")

def writer(writer_id):
    global shared_data
    for i in range(2):
        resource_lock.acquire()
    
        print(f"  [Writer-{writer_id}] WRITING DATA... (Lock held)")
        shared_data += 1
        time.sleep(2)
        print(f" [Writer-{writer_id}] Updated data to: {shared_data}")
        
        resource_lock.release()
        time.sleep(random.uniform(1, 3))

    print(f"✅ [Writer-{writer_id}] Finished execution.")

if __name__ == "__main__":
    print("--- Simulation Started: Readers-Writers Problem ---")
    
    threads = []

    for i in range(3):
        t = threading.Thread(target=reader, args=(i+1,))
        threads.append(t)
        t.start()
    
    t = threading.Thread(target=writer, args=(1,))
    threads.append(t)
    t.start()

    for t in threads:
        t.join()

    print("--- All threads finished. Simulation Complete. ---")