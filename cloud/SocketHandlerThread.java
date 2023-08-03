import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

public class SocketHandlerThread extends Thread {
    private Socket socket;
    private int contentLen;

    public SocketHandlerThread(Socket socket, int contentLen) {
        this.socket = socket;
        this.contentLen = contentLen;
    }

    public static final int getTreeLength(byte fb, byte sb) {
        int first = (((int)(fb & 0b00011111)) << 8);
        int second = (int)sb;

        return first + second;
    }

    public static final int getTrashBitLen(byte b) {
        return (int)((b & 0b11100000) >> 5);
    }

    @Override
    public void run() {
        try {
            InputStream inputStream = this.socket.getInputStream();
            byte firstByte = (byte)(inputStream.read() ^ 0b11111111), secondByte = (byte)(inputStream.read() ^ 0b11111111);
            
            int treeLen = getTreeLength(firstByte, secondByte);
            int trashBits = getTrashBitLen(firstByte);
            
            HuffmanTree ht = new HuffmanTree();
            ht.assemble(inputStream, treeLen);
            
            String result = ht.decompress(inputStream, treeLen, contentLen, trashBits);
            
            System.out.println(result);

            inputStream.close();
            this.socket.close();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }
}
