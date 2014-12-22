/**
* Node class for distributed communication.
*/

#include <signal.h>
#include <iostream>
#include <thread>
#include <hiredis/adapters/libevent.h>
#include "redis.hpp"

using namespace std;

//namespace Redis {

class CommandAsync {
public:
  CommandAsync(string cmd, function<void(string, redisReply*)> callback)
      : cmd(cmd), callback(callback) {}
  string cmd;
  function<void(string, redisReply*)> callback;
};

void connected(const redisAsyncContext *c, int status) {
  if (status != REDIS_OK) {
    printf("Error: %s\n", c->errstr);
    return;
  }
  printf("Connected...\n");
}

void disconnected(const redisAsyncContext *c, int status) {
  if (status != REDIS_OK) {
    printf("Error: %s\n", c->errstr);
    return;
  }
  printf("Disconnected...\n");
}

Redis::Redis(const string &host, const int port) : host(host), port(port), io_ops(0) {

  signal(SIGPIPE, SIG_IGN);
  base = event_base_new();

  c = redisAsyncConnect(host.c_str(), port);
  if (c->err) {
    printf("Error: %s\n", c->errstr);
    return;
  }

  redisLibeventAttach(c, base);
  redisAsyncSetConnectCallback(c, connected);
  redisAsyncSetDisconnectCallback(c, disconnected);

//  // Start the event loop in another thread
//  // TODO the wait one second isn't a great solution
//  thread loop([this] { start(); });
//  loop.detach();
//  this_thread::sleep_for(chrono::milliseconds(1000));
}

Redis::~Redis() {
  redisAsyncDisconnect(c);
}

void Redis::start() {
  event_base_dispatch(base);
}

void Redis::set(std::string key, std::string value) {
  int status = redisAsyncCommand(c, NULL, NULL, "SET %s %s", key.c_str(), value.c_str());
  if (status != REDIS_OK) {
    printf("Error: %s\n", c->errstr);
  }
}

void get_callback(redisAsyncContext *c, void *r, void *privdata) {

  redisReply *reply = (redisReply *) r;
  if (reply == NULL) return;

  string key = "???";
  string value = reply->str;

  auto callback = (void(*)(std::string, std::string)) privdata;
  callback(key, value);
}

void Redis::get(std::string key, void(*callback)(std::string, std::string)) {

  string command = "GET " + key;

  //void (*ptr)(redisAsyncContext *c, void *r, void *privdata) = NULL;

  redisAsyncCommand(c, get_callback, (void*)callback, command.c_str());
}

void command_async_callback(redisAsyncContext* c, void* r, void* privdata) {

  redisReply* reply = (redisReply*)r;
  if(reply == NULL) {
    cerr << "Null reply in callback!" << endl;
    return;
  }

  CommandAsync* cmd_obj = (CommandAsync*)privdata;
  cmd_obj->callback(cmd_obj->cmd, reply);
  delete cmd_obj;
}

void Redis::command_async(std::string cmd, std::function<void(std::string, redisReply*)> callback) {
  CommandAsync* cmd_obj = new CommandAsync(cmd, callback);
  redisAsyncCommand(c, command_async_callback, (void*)cmd_obj, cmd.c_str());
}

//} // End namespace redis
