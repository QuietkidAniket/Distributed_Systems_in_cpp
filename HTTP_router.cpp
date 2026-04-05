#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include "./thread_pool.hpp"

/**
 * @author Aniket Kundu
 * @file HTTP_router.cpp
 * @date 31st March, 2026
 * 
 * @brief A Multithreaded HTTP router example
 * @details Building a Router class that maps URL strings to C++ functions, 
 * mimicking modern web frameworks, and then using it to 
 * 
*/

/**
 * @brief  This class encapsulates the mapping of URL paths to lambda functions.
*/
class Router{
  private:
    std::unordered_map<std::string, std::function<std::string()>> mp;
  public:
    void add_route(const std::string& path, std::function<std::string()> handler){
      mp[path] = std::move(handler);
      std::cout << "[Router] Registered Route : " <<path << "\n";
    }

    std::string route(const std::string& path) const{
      if(mp.find(path) != mp.end()){
        return mp.at(path)();
      }else return "404";
    }
};


void error(const std::string& s){
  std::cerr << "ERROR!" << s << std::endl;
}

/**
 *  @brief The client connection handler sends HTTP based responses based on the requested path
*/
void handle_client(int client_fd, const Router& router){
  char buffer[4096] = {0};
  ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer)-1);

  if(bytes_read <= 0){
    std::cout << "[Worker:" <<std::this_thread::get_id() << "]"<<"Client Disconnected!\n";
    close(client_fd);
    return;
  }
  
  std::string request(buffer);
  std::istringstream iss(request);
  std::string method, path, protocol;
  iss>>method>>path>>protocol;

  std::string body, content_type= "text/html", status= "HTTP/1.1 200 OK\r\n";

  auto res = router.route(path);

  if(res == "404"){
    status = "HTTP/1.1 404 NOT FOUND\r\n";
    body = "<html><body><h1>404- Page Not Found</h1></body><html>";
  }else{
    body = res;
    if(!body.empty() && body[0] == '{'){ // simple check for json
      content_type = "application/json";  
    }
  }

  std::string response = status +
                        "Content-Type: " + content_type + "\r\n" +
                        "Content-Length: " +std::to_string(body.length()) + "\r\n" +
                        "Connection: close\r\n"+
                        "\r\n" +
                        body;
  write(client_fd, response.c_str(), response.length());
  close(client_fd);
}


/**
 * @brief Main thread 
 */
int main(){

  // Setting up the Routing table using the Router class
  Router router;
  router.add_route("/", [](){
    return 
    R"( 
      <html>
      <body>
      <h1> C++ backend Working Fine! 
      </h1>
      </body>
      </html>
     )";

  });

  router.add_route("/api/ping", [](){
    return "{\n\"Message\": \"pong!\", \n\"Status\": \"Active\"\n}";
  });

  router.add_route("/api/data", [](){ // simulating a data driven response
    return "{\"data\": [10,20,30,40]}";
  });

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
    
    pool.enqueue([client_fd, &router](){  // capturing the router by reference 
      handle_client(client_fd, router);
     });
  }

  close(server_fd); 
  return 0;

}