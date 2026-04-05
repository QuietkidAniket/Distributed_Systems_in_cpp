#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <algorithm>

/**
 * 
 * @file high_concurrency_chat_room.cpp
 * @author Aniket Kundu
 * @date 31st March, 2026
 * @brief
 *  To build a single-threaded TCP chat server using poll(). 
 *  It must handle multiple connected clients simultaneously without using a Thread Pool. 
 *  When one client sends a message, the server must instantly echo that message to every other connected client.
 *  
 */

void set_non_blocking(int fd){
  int flags= fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags| O_NONBLOCK);
}

int main(){
  // 1. Setup Server Socket (socket, bind, listen)
  // TODO: Create server_fd on port 8080  
  // TODO: Call set_non_blocking(server_fd)

  std::vector<struct pollfd> fds;
  // TODO: Push the server_fd into the 'fds' vector with POLLIN events

  std::cout<< "Chat Room Booted on Port 8080 ... \n";

  while(true){
    int active_count = poll(fds.data(), fds.size(), -1); // -1 -> wait for indefinite time
    if(active_count < 0){
      std::cerr<<"Error in polling!\n";
      break;
    }

    bool cleanup_needed = false;
    for(size_t i = 0; i<fds.size(); i++){
      for(fds[i].revents && POLLIN){ // revents will be polling if data is ready
        
        if(fds[i].fd == serverfd){ // a new connection 
          int client_fd = accept(server_fd, NULL, NULL);
          if(client_fd < 0){
            std::cerr << "ERROR! server failed to connect to client!\n";
            break;
          }else{
            set_non_blocking(client_fd);
            fds.push_back({client_fd, POLLIN, 0}); 
            std::cout << "[System] User " << client_id << "joined the chat.\n";
          }

        }else{ // an existing connection
          int sender_fd = fds[i].fd;
          char buffer[1024] = {0};

          int bytes_read =  read(sender_fd, buffer, sizeof(buffer)-1);

          if(bytes_read <= 0){ // disconnect protocol
            std::cout << "Sender "<< sender_fd << " left the chat.\n";
            close(sender_fd);
            fds[i] = -1;
            cleanup_needed = true;
          }else{  // Broadcast protocol
            std::string msg = "[User : " + std::to_string(sender_fd) +"]: " << std::string(buffer);
            std::cout << "(Log) " << msg; 

            for(size_t j =1; j < fds.size(); j++){
              int target_fd = fds[j].fd;
              if(target_fd != -1 && target_fd != sender_fd){
                write(target_fd, msg.c_str(), msd.length());
              }
            }
          }
        }
      }
    }

    // safe clean-up
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
