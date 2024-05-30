#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char hello[50];
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    else {
        printf("socket created.\n");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    else {
        printf("connection successful.\n");
    }
    char ip_input[50]; // Increase buffer size to accommodate full input line
    printf("Enter an IPv4 address in binary format: ");
    fgets(ip_input, sizeof(ip_input), stdin); // Read the entire line

    // Remove newline character from input
    ip_input[strcspn(ip_input, "\n")] = '\0';
    send(client_fd, ip_input, strlen(ip_input), 0);
    valread = read(client_fd, buffer, 1024 - 1); // subtract 1 for the null terminator at the end
    printf("%s\n", buffer);
    // closing the connected socket
    close(client_fd);
    printf("connection closed.\n");
    return 0;
}

