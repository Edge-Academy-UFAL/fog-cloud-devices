import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class Cloud {
    public static void main(String[] args) throws IOException {
        int port = 65432;

        ServerSocket serverSocket;
        try {
            serverSocket = new ServerSocket(port);
            System.out.println("Server listening on port " + port);

            while (true) {
                Socket clientSocket = serverSocket.accept();
                System.out.println("New connection established.");

                InputStream inputStream = clientSocket.getInputStream();

                int contentLen = 0;
                for (short i = 0; i < 7; i++) {
                    contentLen *= 10;
                    contentLen += (inputStream.read() ^ 0b11111111);
                }

                // Start a new thread to handle the socket communication
                SocketHandlerThread socketHandler = new SocketHandlerThread(clientSocket, contentLen);
                socketHandler.start();
            }
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }
}
