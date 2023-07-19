from cryptography.fernet import Fernet
import socket, gzip
from pynput import keyboard
import requests, random, json, base64

HOST = 'localhost'
PORT = 65432

def get_random_pokemon_url():
    id = random.randint(1, 150)
    url = f'https://pokeapi.co/api/v2/pokemon/{id}'
    return url

def on_press(key, conn):
    if hasattr(key, 'char'):
        if key.char == 'm':
            print('pressed m')

            with open('key.key', 'r') as file:
                k = file.read()

            url = get_random_pokemon_url()
            print(url)
            data = requests.get(url).json()
            data = bytes(json.dumps(data), encoding='utf-8')

            compacted_data = gzip.compress(data)

            fernet = Fernet(k)
            encrypted = fernet.encrypt(compacted_data)

            conn.sendall(encrypted)
        elif key.char == 'p':
            conn.sendall(b'hi')

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()

    with conn:
        print(f'connected with {addr}')
        listener = keyboard.Listener(on_press=lambda key: on_press(key, conn=conn))
        listener.start()
        while True:
            try:
                pass
            except KeyboardInterrupt:
                listener.stop()
