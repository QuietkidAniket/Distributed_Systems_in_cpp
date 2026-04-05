/**
 * author: Anicetus_7
 * created: 2026-02-04 18:00:00
**/
#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <memory>
#include <print>
using namespace std;

// Global storage for the simulation
map<string, shared_ptr<string>> cache;
vector<pair<string, weak_ptr<string>>> watchers;
mutex cout_mutex; // For thread-safe printing if you detach threads

// Helper to simulate thread work
void process_log(unique_ptr<string> msg_ptr) {
    // TODO: Acquire lock on cout_mutex if necessary
    // Print "LOG: " + *msg_ptr
    lock_guard<mutex> lock(cout_mutex);
    cout << "LOG: " + *msg_ptr;
}

void Solve(){
    cache.clear();
    watchers.clear();

    int q; 
    cin >> q;
    while(q--) {
        string cmd; 
        cin >> cmd;

        if (cmd == "STORE") {
            string key, val;
            cin >> key >> val;
            // TODO: Create a shared_ptr<string> and store in 'cache'
            shared_ptr<string> tmp = make_shared<string>(val);
            cache[key] = tmp;
        } else if (cmd == "WATCH") {
            string key;
            cin >> key;
            // TODO: Look up key in cache. 
            // If found, create a weak_ptr from the shared_ptr and push {key, wp} to 'watchers'.
            if(cache.find(key) != cache.end()){
              shared_ptr<string> tmp = cache[key];
              weak_ptr<string> wp = tmp;
              watchers.push_back({key, wp});
            }
        } 
        else if (cmd == "DELETE") {
            string key;
            cin >> key;
            // TODO: Erase key from 'cache'
            if(cache.find(key) != cache.end()){
              cache.erase(key);
            }
        } 
        else if (cmd == "STATUS") {
            // TODO: Iterate 'watchers'. 
            // Use wp.lock() to check if alive.
            // Print "<key>: Alive: <val>" or "<key>: Expired"
            for(auto p: watchers){
              string key  = p.first;
              weak_ptr<string> ptr = p.second;
              if(auto sp = ptr.lock()){
                cout << key << ": Alive: "<< *sp<< endl;
              }else cout << key << ": Expired: "<<endl;
            }
        }
        else if (cmd == "ASYNC_LOG") {
            string msg;
            cin >> msg;
            // TODO: Create unique_ptr<string>.
            // Spawn a std::thread passing process_log and moving the unique_ptr.
            // Join the thread immediately (for deterministic output in this problem).
            auto uptr = make_unique<string>(msg);
            thread t(process_log, std::move(uptr));
            if(t.joinable())t.join();
        }
    }
}

//|------------------------------------------[MAIN]------------------------------------------|
int32_t main(){
  int t = 1;
  cin>> t;
  for(int i = 1; i <= t; i++) 
    {
        //cout << "Case #" << i << ": \n";
        Solve();
    }
  return 0;
}