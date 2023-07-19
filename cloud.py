from cryptography.fernet import Fernet
import socket, json, gzip

# SOCKET FOR RECIEVING DATA

HOST = 'localhost'
PORT = 65432

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while True:
        data = s.recv(1048576)
        print(f'receieved {data}')

        with open('../../sender/first-iteration/key.key', 'r') as file:
            key = file.read()

        fernet = Fernet(key)
        decrypted = fernet.decrypt(data)

        revealed = gzip.decompress(decrypted)
        print(json.loads(str(revealed, encoding='utf-8')))

        if not data:
            break

# print(f'recieved "{data!r}"')

# with open('../sender/key.key', 'rb') as file:
#     key = file.read()
# with open('../sender/encrypted.encrypted', 'rb') as file:
#     encrypted = file.read()

# fernet = Fernet(key)
# data = fernet.decrypt(encrypted)

# with open('decrypted.json', 'wb') as file:
#     file.write(data)