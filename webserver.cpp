/***************************************************************
Author:         Justin Allen
Major:          Computer Science
Creation Date:  February 3, 2024
Due Date:       TBA
Course:         CPSC 328
Professor:      Schwesinger
Assignment:     #4
Filename:       webserver.cpp
Purpose:        
    This program implements a simple web server that can respond to HTTP GET
    requests. It serves files only from a specified directory and its
    subdirectories, ensuring security by restricting access outside this path.
    The server supports HTML and plain text files, responds with appropriate
    HTTP headers, and provides a verbose mode for debugging.
***************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

using namespace std;

/***************************************************************
Function:       get_content_type
Description:    Determines the MIME type of a file based on its extension.

Parameters:     const string& path - The file path to determine the type.

Return Value:   string - Returns the corresponding MIME type.
***************************************************************/
string get_content_type(const string& path) {
    if (path.find(".html") != string::npos) return "text/html";
    if (path.find(".txt") != string::npos) return "text/plain";
    return "application/octet-stream";
}

/***************************************************************
Function:       is_valid_path
Description:    Checks if the requested file is within the allowed directory.

Parameters:     const string& base_dir - Base directory of the server.
                const string& requested_path - Requested file path.

Return Value:   bool - Returns true if the path is valid, otherwise false.
***************************************************************/
bool is_valid_path(const string& base_dir, const string& requested_path) {
    char real_base[PATH_MAX], real_requested[PATH_MAX];
    realpath(base_dir.c_str(), real_base);
    realpath(requested_path.c_str(), real_requested);
    return strstr(real_requested, real_base) == real_requested;
}

/***************************************************************
Function:       handle_client
Description:    Handles a client request by parsing the HTTP request,
                validating file access, and sending an appropriate response.

Parameters:     int client_socket - The socket descriptor for the client.
                const string& base_dir - The base directory for file serving.
                bool verbose - Flag to enable verbose logging.

Return Value:   void
***************************************************************/
void handle_client(int client_socket, const string& base_dir, bool verbose) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }

    if (verbose) cout << "Received request:\n" << buffer << endl;

    stringstream request(buffer);
    string method, path, version;
    request >> method >> path >> version;

    if (method != "GET") {
        string response = "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }

    if (path == "/") path = "/index.html";
    string full_path = base_dir + path;
    
    if (!is_valid_path(base_dir, full_path)) {
        string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }

    struct stat file_stat;
    if (stat(full_path.c_str(), &file_stat) != 0 || !S_ISREG(file_stat.st_mode)) {
        string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }

    ifstream file(full_path, ios::binary);
    if (!file.is_open()) {
        string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    }

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    string content_type = get_content_type(full_path);
    
    stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << content.size() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << content;

    send(client_socket, response.str().c_str(), response.str().size(), 0);
    close(client_socket);
}

int main(int argc, char* argv[]) {
    int port = -1;
    string base_dir = "./";
    bool verbose = false;

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-p" && i + 1 < argc) {
            try {
                port = stoi(argv[++i]);
            } catch (const invalid_argument&) {
                cerr << "Error: Invalid port number." << endl;
                return 1;
            }
        } else if (string(argv[i]) == "-d" && i + 1 < argc) {
            base_dir = argv[++i];
        } else if (string(argv[i]) == "-v") {
            verbose = true;
        }
    }

    struct stat info;
    if (stat(base_dir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        cerr << "Error: Directory '" << base_dir << "' does not exist." << endl;
        return 1;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return 1;
    }

    if (listen(server_socket, 10) == -1) {
        perror("listen");
        return 1;
    }

    cout << "Server is running on port " << port << " serving directory: " << base_dir << endl;

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == -1) {
            perror("accept");
            continue;
        }
        handle_client(client_socket, base_dir, verbose);
    }

    close(server_socket);
    return 0;
}
