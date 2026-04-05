#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include "./thread_pool.hpp"
#include "./ThreadSafeCache.hpp"

class Redis{
  private:
  const std::string SET_REQUEST = "SET";
  const std::string GET_REQUEST = "GET";
  const std::string DELETE_REQUEST =  "DELETE";
  std::unordered_map<std::string, std::string> cache;

  void error(const std::string& msg){
    std::cerr << "Error! : " << msg<< std::endl;
  }

  void set_non_blocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  }

  std::string set(std::string& key, std::string& val){
    if(key.empty())return "ERROR!\n";
    cache[key] = val;
    return "OK";
  }
  
  std::string get(std::string& key){
    if(cache.find(key) == cache.end())return "(nill)\n";
    return cache[key];
  }
  
  std::string delete_(std::string& key){
    if(cache.find(key) == cache.end())return "ERROR\n";
    cache.erase(key);
    return "OK";
  }
  public:

  int run(){
    // socket creation
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
      error("Socket creation failed!");
      return 1;
    }

    set_non_blocking(server_fd);
    // socket binding
    struct sockaddr_in  address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080); 
    bind(server_fd, (struct sockaddr*) &address, sizeof(address));

    // listening
    listen(server_fd, 10);

    // socket connections / files descriptors
    std::vector<struct pollfd> fds;
    fds.push_back({server_fd, POLLIN, 0});

    // poll event loop
    while(true){
      poll(fds.data(), fds.size(), -1);

      bool cleanup_needed = false;

      for(size_t i = 0; i < fds.size(); i++){
        if(fds[i].revents & POLLIN){
          if(fds[i].fd == server_fd){ // incoming new client connection via the server socket
            int client_fd = accept(server_fd, NULL, NULL);
            if(client_fd >= 0){
              set_non_blocking(client_fd);
              fds.push_back({client_fd, POLLIN, 0});
              std::cout << "[System] User " <<client_fd << " connected.\n";
            }
          }else{
            int sender_fd = fds[i].fd;
            char buffer[4096] = {0};
            int bytes_read = read(sender_fd,  buffer, sizeof(buffer)-1);
            if( bytes_read <= 0){
              close(sender_fd);
              fds[i].fd=  -1;
              cleanup_needed = true;
              std::cout << "[System] User " << sender_fd << "has disconnected.\n";
            }else{ 

              std::string method, key, val;
              std::istringstream iss(buffer);
              iss >> method;
              if(method == SET_REQUEST){
                iss>> key>>val;
                std::string response = set(key, val);;
                write(sender_fd, response.c_str(),  response.length());
              }else if(method == GET_REQUEST){
                iss>> key;
                std::string response = get(key);
                // we just send the response back to the sender
                write(sender_fd, response.c_str(),  response.length());
              }else if(method == DELETE_REQUEST){
                iss>>key;
                std::string response = delete_(key);
                write(sender_fd, response.c_str(),  response.length());
              }else{
                error("Invalid Request! from user "  + std::to_string(sender_fd));
                std::string response = "ERROR";
                write(sender_fd, response.c_str(),  response.length());
              }
            }
          }
        }
      }

      // cleanup is done here
      if(cleanup_needed){
        auto it = std::remove_if(fds.begin(), fds.end(), [](const struct pollfd& p){
          return p.fd == -1;
        });
        fds.erase(it, fds.end());
      }
    }

    close(server_fd);
    return 0;
  } 

};

int main(){
  Redis server;
  return server.run();
}