//server:
//CRC_Server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define PORT 8086
#define GEN_POLY "1011"
#define N strlen(GEN_POLY)

char data[28];
char check_value[28];
int data_length, i, j;

void XOR() {
    for (j = 1; j < N; j++)
        check_value[j] = (check_value[j] == GEN_POLY[j]) ? '0' : '1';
}

void crc() {
    for (i = 0; i < N; i++)
        check_value[i] = data[i];

    do {
        if (check_value[0] == '1')
            XOR();

        for (j = 0; j < N - 1; j++)
            check_value[j] = check_value[j + 1];

        check_value[j] = data[i++];
    } while (i <= data_length + N - 1);
}

void receiver() {
    printf("Data received: %s\n", data);

    crc();

    for (i = 0; (i < N - 1) && (check_value[i] != '1'); i++);

    if (i < N - 1)
        printf("\nError detected\n\n");
    else
        printf("\nNo error detected\n\n");
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int client_address_length;

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    client_address_length = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_length);
    if (client_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected by client\n");

    // Receive the data from the client
    memset(data, 0, sizeof(data));
    if (recv(client_socket, data, sizeof(data), 0) < 0) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }
    
    receiver();

    // Close the client socket
    close(client_socket);

    // Close the server socket
    close(server_socket);

    return 0;
}








//CRC_Client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 8086
#define GEN_POLY "1011"
#define N strlen(GEN_POLY)

char data[28];
char check_value[28];
int data_length, i, j;

void XOR() {
    for (j = 1; j < N; j++)
        check_value[j] = (check_value[j] == GEN_POLY[j]) ? '0' : '1';
}

void crc() {
    for (i = 0; i < N; i++)
        check_value[i] = data[i];

    do {
        if (check_value[0] == '1')
            XOR();

        for (j = 0; j < N - 1; j++)
            check_value[j] = check_value[j + 1];

        check_value[j] = data[i++];
    } while (i <= data_length + N - 1);
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char message[BUFFER_SIZE];

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server details
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr)) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Get the message from the user
    printf("Enter message to be transmitted: ");
    scanf("%s", data);

    data_length = strlen(data);

    for (i = data_length; i < data_length + N - 1; i++)
        data[i] = '0';

    printf("\n----------------------------------------");
    printf("\nData padded with n-1 zeros: %s", data);
    printf("\n----------------------------------------");

    crc();

    printf("\nCRC or Check value is: %s", check_value);

    for (i = data_length; i < data_length + N - 1; i++)
        data[i] = check_value[i - data_length];

    printf("\n----------------------------------------");
    printf("\nFinal data to be sent: %s", data);
    printf("\n----------------------------------------\n");

    // Send the data to the server
    if (send(client_socket, data, sizeof(data), 0) < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    // Close the client socket
    close(client_socket);

    return 0;
}

