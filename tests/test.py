from multiprocessing import Process
from threading import Thread, Semaphore
import time

# Semáforo para controlar acceso a la cola de mensajes
message_queue_semaphore = Semaphore()

# Cola de mensajes compartida
message_queue = []

def handle_user(user_id):
    while True:
        with message_queue_semaphore:
            if message_queue:
                message = message_queue.pop(0)
                print(f"User {user_id} received message: {message}")
        time.sleep(1)

def chat_instance(instance_id):
    print(f"Chat Instance {instance_id} started")
    user_threads = []
    for user_id in range(5):
        t = Thread(target=handle_user, args=(user_id,))
        t.start()
        user_threads.append(t)
    
    for t in user_threads:
        t.join()

def monitor():
    print("Starting monitor")
    chat_processes = []
    for i in range(3):
        p = Process(target=chat_instance, args=(i,))
        p.start()
        chat_processes.append(p)

    for p in chat_processes:
        p.join()

if __name__ == "__main__":
    monitor()

