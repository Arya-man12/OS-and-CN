#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#define PORT 8080

// Function to print the binary representation of an 8-bit number
void printBinary(int num) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
}

int validate_ip(char *ip_str) {
    char ip_address[50];
    strcpy(ip_address,ip_str);
    int j = 0; // Index for the new string
    int x=0;
    int y=0;
    int count=0;
    // Iterate over each character of the original string
    for (int k = 0; ip_address[k] != '\0'; k++) {
        // If the character is not a dot, copy it to the new string
        if (ip_address[k] != '.') {
            count++;
        }
        else x++;
        if(ip_address[k]!='0' && ip_address[k]!='1' && ip_address[k] != '.') y++;
    }
    if(y!=0 || x!=3 || count!=32) return 0;

    char *token;
    int binary[4];
    

    // Make a copy of the string because strtok modifies the original string
    char ip_copy[50];
    strcpy(ip_copy, ip_str);

    // Get the first token
    token = strtok(ip_copy, ".");
    
    // Walk through other tokens
    int i = 0;
    while (token != NULL && i < 4) {
        binary[i++] = strtol(token, NULL, 2);
        token = strtok(NULL, ".");
        
    }
    
    if ((binary[0] >= 0 && binary[0] <= 0b11111111) &&
        (binary[1] >= 0 && binary[1] <= 0b11111111) &&
        (binary[2] >= 0 && binary[2] <= 0b11111111) &&
        (binary[3] >= 0 && binary[3] <= 0b11111111)) {
        return 1; // Valid IP address
    }
    
    return 0; // Invalid IP address
}

// Function to calculate subnet mask, network ID, host ID, and broadcast ID
void calculate_ip_info(char *ip_str) {
    uint32_t ip, subnet_mask, network_id, host_id, broadcast_id;
    char class;
    char ip_address[50];
    strcpy(ip_address,ip_str);
    char ip_without_dots[sizeof(ip_address)]; // Assuming the size won't exceed that of the original string
    int j = 0; // Index for the new string

    // Iterate over each character of the original string
    for (int k = 0; ip_address[k] != '\0'; k++) {
        // If the character is not a dot, copy it to the new string
        if (ip_address[k] != '.') {
            ip_without_dots[j++] = ip_address[k];
        }
    }
    ip_without_dots[j] = '\0'; // Null-terminate the new string
    unsigned int binary_value = strtol(ip_without_dots, NULL, 2);
    char bv[20];
    sprintf(bv, "%u", binary_value);
    sscanf(bv, "%u", &ip);
    printf("Class: ");
    if ((ip & 0x80000000) == 0) {
        printf("A\n");
        class='A';
    } else if ((ip & 0xC0000000) == 0x80000000) {
        printf("B\n");
        class='B';
    } else if ((ip & 0xE0000000) == 0xC0000000) {
        printf("C\n");
        class='C';
    } else if ((ip & 0xF0000000) == 0xE0000000) {
        printf("D\nFor research purposes.\n");
        class='D';
    } else {
        printf("E\nFor research purposes.\n");
        class='E';
    }
    // Subnet mask determined based on IP class
    if ((ip & 0x80000000) == 0) {
        subnet_mask = 0xFF000000;
    } else if ((ip & 0xC0000000) == 0x80000000) {
        subnet_mask = 0xFFFF0000;
    } else if ((ip & 0xE0000000) == 0xC0000000) {
        subnet_mask = 0xFFFFFF00;
    } else {
        subnet_mask = 0xFFFFFFFF;
    }

    network_id = ip & subnet_mask;
    host_id = ip & ~subnet_mask;
    broadcast_id = network_id | ~subnet_mask;
    if(class!='D' && class!='E') {
        printf("Subnet Mask: ");
        for (int i = 0; i < 4; i++) {
            printBinary((subnet_mask >> (24 - i*8)) & 0xFF);
            if (i < 3) printf(".");
        }
        printf("\n");
        
        printf("Network ID: ");
        for (int i = 0; i < 4; i++) {
            printBinary((network_id >> (24 - i*8)) & 0xFF);
            if (i < 3) printf(".");
        }
        printf("\n");
        
        printf("Host ID: ");
        for (int i = 0; i < 4; i++) {
            printBinary((host_id >> (24 - i*8)) & 0xFF);
            if (i < 3) printf(".");
        }
        printf("\n");
        
        printf("Broadcast ID: ");
        for (int i = 0; i < 4; i++) {
            printBinary((broadcast_id >> (24 - i*8)) & 0xFF);
            if (i < 3) printf(".");
        }
        printf("\n");
    }
}

// Function to determine the IP address class
char get_ip_class(char *ip_str) {
    uint32_t ip;
    sscanf(ip_str, "%u", &ip);
    if ((ip & 0x80000000) == 0) {
        return 'A';
    } else if ((ip & 0xC0000000) == 0x80000000) {
        return 'B';
    } else if ((ip & 0xE0000000) == 0xC0000000) {
        return 'C';
    } else if ((ip & 0xF0000000) == 0xE0000000) {
        return 'D';
    } else {
        return 'E';
    }
}

int main(int argc, char const* argv[])
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "IP Address received";
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    else {
        printf("socket created.\n");
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    else {
        printf("bind successful.\n");
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    else {
        printf("listen successful.\n");
    }
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    else {
        printf("accept successful.\n");
    }
    valread = read(new_socket, buffer, 1024 - 1); // subtract 1 for the null terminator at the end
    printf("%s\n", buffer);

    if (validate_ip(buffer)) {
        calculate_ip_info(buffer);
    } else {
        printf("Invalid IPv4 address format.\n");
    }

    send(new_socket, hello, strlen(hello), 0);
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    close(server_fd);
    printf("connection closed.\n");
    return 0;
}


