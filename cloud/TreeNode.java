public class TreeNode {
    public byte b;
    public TreeNode left;
    public TreeNode right;
    
    public TreeNode(byte b, TreeNode left, TreeNode right) {
        this.b = b;
        this.left = left;
        this.right = right;
    }

    public TreeNode(byte b) {
        this(b, null, null);
    }

    public boolean isLeaf() {
        return this.left == null && this.right == null;
    }
}
