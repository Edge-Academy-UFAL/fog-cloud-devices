import java.io.IOException;
import java.io.InputStream;

public class HuffmanTree {
    public TreeNode root;

    public HuffmanTree() {
        this.root = null;
    }

    public String decompress(InputStream fis, int treeLen, int fileLen, int trashBits) throws IOException {
        String result = "";
        int i = 2 + 7 + treeLen;

        byte b;
        TreeNode node = this.root;
        boolean ended = false;
        while (true) {
            b = (byte)fis.read();
            b = (byte)(b ^ 0b11111111);

            for (int j = 7; j >= 0; j--){
                if (i == fileLen - 1 && j < trashBits) {
                    ended = true;
                    break;
                }
                
                if ((b & (0b00000001 << j)) == 0) {
                    node = node.left;
                }
                else {
                    node = node.right;
                }

                if (node.isLeaf()) {
                    result = result.concat(Character.toString((char)node.b));
                    node = this.root;
                }
            }

            if (ended) break;
            i++;
        }

        return result;
    }

    private TreeNode assembleRecursive(TreeNode parent, InputStream fis, IntHolder i, int len) throws IOException {
        if (i.i >= len) return null;
        
        byte b = (byte)(fis.read() ^ 0b11111111);
        i.i++;

        boolean isLeaf = (b != (byte)'*');

        if (b == (byte)'\\') {
            b = (byte)fis.read();
            i.i++;
        }

        TreeNode current = new TreeNode(b);

        if (isLeaf) {
            current.left = null;
            current.right = null;
        }
        else {
            current.left = assembleRecursive(current, fis, i, len);
            current.right = assembleRecursive(current, fis, i, len);
        }

        return current;
    }

    public void assemble(InputStream fis, int len) throws IOException {
        IntHolder i = new IntHolder(2);
        this.root = assembleRecursive(this.root, fis, i, 2 + len);
    }

    public void printTreeRecursive(TreeNode current) {
        if (current == null) return;

        System.out.print((char)current.b);
        printTreeRecursive(current.left);
        printTreeRecursive(current.right);
    }

    public void printTree() {
        printTreeRecursive(this.root);
        System.out.println();
    }
}