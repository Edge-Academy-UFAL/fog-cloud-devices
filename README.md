# IoT-Fog-Cloud Devices Emulator 

This repository contains code that emulates IoT, Fog and Cloud devices, commonly used in the context of Internet of Things. The IoT device is emulated by the iot_device.py script, that simply sends JSON data to the Fog. The Fog is emulated by the fog.c program (emulating some kind of embedded system), which receives data from the IoT device, compresses encrypts it, then sends it to the Cloud. The Cloud is emulated by the Cloud.java program, which receives all incoming data from the Fog, decrypts and decompresses it, then print the data on the standart output stream.

## IoT Device

For testing purposes, the IoT Device sript sends a JSON file containing information about a random 1º generation Pokemon. This data is obtained through an API request to PokeAPI. This data is then sent to the Fog through the 65431 port.

Python was used for its simplicity, since the script's main purpose is sending data, which means that there's no specifically right programming language for this purpose.

## Fog Device

The Fog device is written in C and listens to the 65431 port to receive data. The received data is then compressed utilizing Huffman Algorithm and encrypted by applying a NOT gate to all bits of the compressed data. The program sends this compressed and encrypted data to the Cloud through the 65432 port.

This type of encryption because the data being transmitted is not supposed to be sensitive, and so the encryption being done is just for serving its basic role of making it difficult for others to gather information from the data. 

## Cloud Device

The Cloud device is written in Java and listens to the 65432 port for data. The received data is decrypted by applying a NOT gate to all bits and then decompressed utilizing the header instructions for creating the Huffman Tree needed for the decompression. Finally, the original data is shown on screen through a simple println statement.
