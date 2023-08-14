from requests import get
from random import randint
import socket
import json

URL = 'https://pokeapi.co/api/v2/pokemon/'
IP = 'localhost'
PORT = 65431

def get_random_pokemon():
    id = randint(1, 150)
    url = URL + str(id)
    
    return json.dumps(get(url).json())


if __name__ == '__main__':
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        sock.connect((IP, PORT))
        print('Connected to server')

        pokemon: str = get_random_pokemon()
        sock.sendall(pokemon.encode())
        print(f'number of bytes sent: {len(pokemon)}')
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running.")
    except Exception as e:
        print("Error occurred:", e)
    finally:
        sock.close()
        print("Connection closed.")
    
