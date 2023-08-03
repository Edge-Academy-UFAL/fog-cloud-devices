#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUMBER 65432
#define BUFFER_SIZE 1048576
#define MAX_BYTES 256

typedef unsigned char byte;
typedef long long ll;

ll frequency[MAX_BYTES] = {0};
char paths[MAX_BYTES][17];

typedef struct Node {
    struct Node *next;
    struct Node *left;
    struct Node *right;
    ll freq;
    byte b;
} Node;

Node *create_node(byte b, ll f) {
    Node *new_node = malloc(sizeof(Node));
    new_node->freq = f;
    new_node->left = NULL;
    new_node->next = NULL;
    new_node->right = NULL;
    new_node->b = b;

    return new_node;
}

void insert_in_list(Node **head, Node *node) {
    if (*head == NULL || node->freq <= (*head)->freq) {
        node->next = *head;
        *head = node;
        return;
    }

    Node *check;
    for (check = *head; check->next != NULL; check = check->next) {
        if (node->freq <= check->next->freq) {
            node->next = check->next;
            check->next = node;
            return;
        }
    }

    check->next = node;
}

void assemble_linked_list(Node **head) {
    for (short i = 0; i < MAX_BYTES; i++) {
       if (frequency[i] != 0) {
           insert_in_list(head, create_node(i, frequency[i]));
       }
    }
}

void print_list(Node *node) {
    for (; node != NULL; node = node->next) {
        printf("freq: %lld, byte: %d\n", node->freq, node->b);
    }
}

Node *pop(Node **head) {
    if (*head == NULL) return NULL;

    Node *result = *head;
    *head = result->next;

    return result;
}

byte is_leaf(Node *node) {
    return (node->left == NULL && node->right == NULL);
}

void print_tree(Node *node) {
    if (node == NULL) return;

    if ((node->b == (byte)'*' || node->b == (byte)'\\') && is_leaf(node)) {
        printf("\\");
    }
    printf("%c", node->b);
    print_tree(node->left);
    print_tree(node->right);
}

void assemble_huffman_tree(Node **root) {
    byte ran_once = 0;

    while (!ran_once || (*root)->next != NULL) {
        Node *left = pop(root);
        Node *right = pop(root);

        ll freq = (left == NULL ? 0 : left->freq) + (right == NULL ? 0 : right->freq);

        Node *parent = create_node((byte)'*', freq);
        parent->left = left;
        parent->right = right;

        insert_in_list(root, parent);
        
        ran_once = 1;
    }
}

void map_byte_paths(Node *node, char str[], byte len) {
    if (node == NULL) return;

    if (is_leaf(node)) {
        strcpy(paths[node->b], str);
    }
    else {
        char left_str[len + 1];
        char right_str[len + 1];

        strcpy(left_str, str);
        strcpy(right_str, str);
        strcat(left_str, "0");
        strcat(right_str, "1");
    

        map_byte_paths(node->left, left_str, len + 1);
        map_byte_paths(node->right, right_str, len + 1);
    }
}

void free_tree(Node *node) {
    if (node == NULL) return;
    
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

int get_tree_size(Node *node) {
    if (node == NULL) return 0;

    return 1 +
        (is_leaf(node) && (node->b == (byte)'*' || node->b == (byte)'\\')) +
        get_tree_size(node->left) +
        get_tree_size(node->right);
}

void calculate_first_two_bytes(Node *root, byte *first_byte, byte *second_byte, ll total_bits) {
    byte trash_bits = (8 - (total_bits % 8)) % 8;
    int tree_size = get_tree_size(root);

    *first_byte = (trash_bits << 5);
    *first_byte = *first_byte | (byte)((tree_size >> 8) & 0b00011111);
    *second_byte = (byte)tree_size;
}

void write_tree(Node *node, byte str[], short *aux) {
    if (node == NULL) return;

    if (is_leaf(node) && (node->b == (byte)'*' || node->b == (byte)'\\')) {
        str[(*aux)++] = '\\';
    }
    str[(*aux)++] = node->b;

    write_tree(node->left, str, aux);
    write_tree(node->right, str, aux);
}

ll get_compacted_message_bits() {
    ll result = 0;

    for (short i = 0; i < MAX_BYTES; i++) {
        if (frequency[i] == 0) continue;
        
        result += frequency[i] * strlen(paths[i]);
    }

    return result;
}

void get_compacted_message_str(byte compacted[], byte buffer[], ll initial, ll len) {
    byte b = 0;
    char j = 7;
    ll i_compacted = initial;

    for (ll i = 0; i < len; i++) {
        char *path = paths[buffer[i]];
        for (byte i_path = 0; path[i_path] != '\0'; i_path++) {
            if (path[i_path] == '1') b = b | (1 << j);
            j--;

            if (j < 0) {
                compacted[i_compacted++] = b;
                b = 0;
                j = 7;
            }
        }
    }

    if (j != 7) compacted[i_compacted] = b;
}

void write_content_size(byte content[], ll content_size) {
    int t = content_size;
    for (short i = 6; i >= 0; i--) {
        content[i] = (byte)(t % 10);
        t /= 10;
    }
}

void send_to_cloud(byte content[], ll content_len) {
    int sockfd;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUMBER);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to the server");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send a message to the server
    // printf("sending %d bytes\n", content_len);
    for (ll i = 0 ; i < content_len; i++) {
        content[i] = (content[i] ^ 0b11111111);
        printf("%c", content[i]);
    }
    printf("\n\n");

    send(sockfd, content, content_len, 0);

    // Close the socket
    close(sockfd);
}

void compress_file_and_send(byte buffer[], ll file_len) { 
    Node *head = NULL;
    for (ll i = 0; i < file_len; i++) {
        frequency[buffer[i]]++;
    }

    assemble_linked_list(&head);
    assemble_huffman_tree(&head);

    map_byte_paths(head, "", 1);

    byte first_byte, second_byte;
    ll compacted_message_bits = get_compacted_message_bits();
    short tree_size = get_tree_size(head);

    calculate_first_two_bytes(head, &first_byte, &second_byte, compacted_message_bits);
    
    byte huffman_tree[tree_size];
    short aux = 0;
    write_tree(head, huffman_tree, &aux);
 
    ll total_size = compacted_message_bits / 8 + (compacted_message_bits % 8 != 0) + 2 + tree_size + 7;
    byte compacted_file[total_size];
    ll initial;

    write_content_size(compacted_file, total_size);

    compacted_file[7] = first_byte;
    compacted_file[8] = second_byte;

    for (initial = 9; initial < 2 + 7 + tree_size; initial++) {
        compacted_file[initial] = huffman_tree[initial - 2 - 7];
    }

    get_compacted_message_str(compacted_file, buffer, initial, file_len);
    
    free_tree(head);

    memset(frequency, 0, MAX_BYTES * sizeof(ll));
    for (short i = 0; i < MAX_BYTES; i++) {
        memset(paths[i], 0, 17);
    }

    send_to_cloud(compacted_file, total_size);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Please specify what must be sent to the cloud like so:\n");
        printf("\t./fog <name of file and extension>\n");
        return 0;
    }

    FILE *file = fopen(argv[1], "rb");

    if (file == NULL) {
        printf("File not found\n");
        return 0;
    }

    ll file_len;
    fseek(file, 0, SEEK_END);
    file_len = ftell(file);
    fseek(file, 0, SEEK_SET);

    byte buffer[file_len];
    fread(buffer, 1, file_len, file);

    fclose(file);

    compress_file_and_send(buffer, file_len);

    return 0;
}