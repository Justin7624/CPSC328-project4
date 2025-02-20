# CPSC328-project4
# Simple Web Server

## Overview
This project implements a **simple HTTP web server** that responds to **HTTP GET requests** using **sockets**. The server serves files from a specified directory and its subdirectories, ensuring security by restricting access outside this path. The server supports serving **HTML and plain text files**, handles **404 errors**, and provides a **verbose mode** for debugging.

## Features
- Handles **HTTP GET requests**.
- Serves files from a **specified directory** and its subdirectories.
- Prevents unauthorized file access outside the allowed directory.
- Supports **HTML and plain text** file types.
- Implements **error handling for missing or invalid files**.
- Includes **verbose mode** for debugging requests.

## Usage
### Compilation
To compile the project, use the provided **Makefile**:
```bash
make
```
This will generate an executable named `webserver`.

### Running the Server
Run the server with the following command-line arguments:
```bash
./webserver -p <port> -d <directory> [-v]
```
#### Example:
```bash
./webserver -p 8080 -d ./public -v
```
This starts the server on **port 8080**, serving files from the `./public` directory, with **verbose mode** enabled.

### Cleaning Up
To remove compiled files:
```bash
make clean
```

## Implementation Details
### Steps Performed:
1. **Create a socket** using `socket()`.
2. **Bind the socket** to the specified port using `bind()`.
3. **Listen for incoming connections** with `listen()`.
4. **Accept client connections** using `accept()`.
5. **Receive the HTTP request** using `recv()`.
6. **Parse the request** to extract the requested file.
7. **Validate the file path** to ensure it is within the allowed directory.
8. **Serve the file** with the correct `Content-Type` header.
9. **Send the HTTP response** to the client.
10. **Close the client connection**.

### Command-line Arguments:
- `-p <port>`: Specify the server port (required).
- `-d <directory>`: Set the directory to serve files from (default: `./`).
- `-v`: Enable verbose mode (prints received HTTP requests).

## Code Structure
- `webserver.cpp` - Main program implementing the server logic.
- `Makefile` - Compilation and cleanup rules.

## Error Handling
- Ensures **valid command-line arguments**.
- Prevents serving **files outside the allowed directory**.
- Handles **missing or inaccessible files with 404 errors**.
- Checks for **socket creation and binding failures**.

## Author
- **Justin Allen**
