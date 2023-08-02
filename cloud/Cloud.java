import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class Cloud {
    public static final int getTreeLength(byte fb, byte sb) {
        int first = (((int)(fb & 0b00011111)) << 8);
        int second = (int)sb;

        return first + second;
    }

    public static final int getTrashBitLen(byte b) {
        return (int)((b & 0b11100000) >> 5);
    }

    public static void main(String[] args) throws IOException {
        int port = 65432;
        // int buffer = 1048576;

        try {
            ServerSocket serverSocket = new ServerSocket(port);
            System.out.println("Server started. Listening on port: " + port);

            Socket clientSocket = serverSocket.accept();

            // Get the InputStream from the client's socket
            InputStream inputStream = clientSocket.getInputStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));

            int contentLen = 0;
            for (short i = 0; i < 7; i++) {
                contentLen *= 10;
                contentLen += inputStream.read();
            }
            byte firstByte = (byte)inputStream.read(), secondByte = (byte)inputStream.read();
            
            int treeLen = getTreeLength(firstByte, secondByte);
            int trashBits = getTrashBitLen(firstByte);
            
            HuffmanTree ht = new HuffmanTree();
            ht.assemble(inputStream, treeLen);
            
            String result = ht.decompress(inputStream, treeLen, contentLen, trashBits);
            
            System.out.println(result);

            // Close the socket and other resources
            reader.close();
            clientSocket.close();
            serverSocket.close();
        }
        catch(IOException e) {
            e.printStackTrace();
        }
    }
}
