#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include "./thread_pool.hpp"

/**
 * @author Aniket Kundu
 * @file multithreaded_tcp_echo_server.cpp
 * @date 30th March, 2026
 * 
 * @brief A Multithreaded TCP Socket Programming example
 * @details The main thread submits a task - to handle the client_fd received 
 * via accept() for doing the read and write - to the  thread pool
 * and multiple connections can be created and handled now
 * 
*/

void error(const std::string& s){
  std::cerr << "ERROR!" << s << std::endl;
}


void handle_client(int client_fd){
  char buffer[1024] = {0};
  std::string welcome_txt  = "Welcome to the multithreaded TCP socket server! Type Quit to exit.\n";
  write(client_fd, welcome_txt.c_str(), welcome_txt.length());

  while(true){
    memset(buffer, 0, sizeof buffer);

    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer)-1);
    if(bytes_read <= 0){
      std::cout << "[Worker:" <<std::this_thread::get_id() << "]"<<"Client Disconnected!\n";
      break;
    }
    
    std::string received(buffer);
    if(received.find("QUIT") != std::string::npos){
      std::string goodbye = "GOODBYE!\n";
      write(client_fd, goodbye.c_str(), goodbye.length());
      std::cout << "[Worker:" <<std::this_thread::get_id() << "]"<<"Client Gracefully Quit!\n";
      break;
    }
    std::transform(received.begin(), received.end(), received.begin(), [](unsigned char c){
      return std::toupper(c);
    });
    std::string response = "SERVER ECHO:"+ received;
    write(client_fd, response.c_str(), response.length());

  }
  close(client_fd);
}


/**
 * @brief Main thread 
 */
int main(){
  // 1. create a new socket 
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);  

  if(server_fd == -1){
    error("Error in Socket Creation!");
    return 1;
  }

  // 2. Configure the Port and IP address
  struct sockaddr_in address;
  address.sin_family  = AF_INET;
  address.sin_addr.s_addr =  INADDR_ANY;
  address.sin_port = htons(8080); // htons handles the endian-ness

  // 3. Bind the socket and the server starts
  if(bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0 ){
    error("Error in Binding the Socket! Port might already be in use!");
    return 1;
  }

  // 4. Listen to the socket
  if(listen(server_fd, 10) < 0){
    close(server_fd);
    error("Error in Listening to the socket!");
    return 1;
  }
  
  // Create a Thread pool of 4 threads
  ThreadPool pool(4);

  // 5. Accepting an incoming new connection request 
  while(true){
    int addrlen = sizeof(address);
    int client_fd = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen);
    if(client_fd < 0){
      error("Error, accept failed! Terminating the Server (for debugging purposes)!");
      close(server_fd);
      return 1;
    }

    std::cout << "New Connection accepted! FD : " << client_fd<< ". Handing to pool\n";
    // if the client socket is successfully created 
    // then just assign the socket as a new task to the thread pool
    pool.enqueue([client_fd](){ 
      // we pass a lambda which runs the handle_client function by giving 
      // it the client_fd as the argument, we cannot just pass handle_client function
      // with an argument directly as a lambda as the thread pool enqueue function 
      // doesnt accept a lambda with arguments
      handle_client(client_fd);
     });
  }

  close(server_fd); 
  // no need to manually close the TCP client sockets since on closing 
  // the server socket, the server socket sends a last FIN (finish) packet
  // to initiate a graceful closure of all client connections
  return 0;

}