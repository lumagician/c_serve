/**
 * 
 * Server side socket
 *
 * 1. socket()
 * 2. bind()
 * 3. listen()
 * 4. accept()
 * 
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT 1234
#define HOSTNAME "SOME_HOSTNAME"

int main(int argc, char *argv[])
{
    struct sockaddr_in address;
    ssize_t valread;
    int error;
    char buffer[1024] = {0};
    socklen_t addrlen = sizeof(address);
    char* hello = "Hello from server\n";

    // create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind to socket
    error = bind(sock_fd, (struct sockaddr*)&address, sizeof(address));

    
    // start listening
    listen(sock_fd, 3);

    // accept a connection
    int newsock_fd = accept(sock_fd, (struct sockaddr*)&address, &addrlen);

    valread = read(newsock_fd, buffer, 1024 -1);
    printf("%s", buffer);

    // get filename from GET request (start at 5th character and stop at space)
    char* filename = malloc(1024);
    int i = 0;
    while (buffer[i + 5] != ' ') {
        filename[i] = buffer[i + 5];
        i++;
    }
    filename[i] = '\0';


    printf("Filename: %s\n", filename);

    // open file
    FILE* file = fopen(filename, "r");

    // read file
    char* file_buffer = malloc(1024);
    size_t file_size = fread(file_buffer, 1, 1024, file);

    // calculate content length
    char* content_length = malloc(1024);
    sprintf(content_length, "Content-Length: %ld\n", file_size);


    // send header
    send(newsock_fd, "HTTP/1.1 200 OK\n", 16, 0);
    send(newsock_fd, "Content-Type: text/html\n", 24, 0);
    send(newsock_fd, content_length, strlen(content_length), 0);
    send(newsock_fd, "\n", 1, 0);

    // send file
    send(newsock_fd, file_buffer, file_size, 0);

    // send two newlines to end connection
    send(newsock_fd, "\n\n", 2, 0);

    // close connection
    close(newsock_fd);
    close(sock_fd);

    return 0;
}