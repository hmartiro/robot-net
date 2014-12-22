/**
* Hiredis wrapper for C++.
*/

#pragma once

#include <functional>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>

//namespace redis {

class Redis {

public:

  Redis(const std::string& host, const int port);
  ~Redis();

//  void publish(std::string channel, std::string msg);
//  void subscribe(std::string channel, std::function<void(std::string channel, std::string msg)> callback);
//  void unsubscribe(std::string channel);

  void set(std::string key, std::string value);
  void get(std::string key, void(*callback)(std::string, std::string));

  void command_async(std::string command, std::function<void(std::string, redisReply* r)> callback);

  void start();

private:

  // Redis server
  std::string host;
  int port;

  // Number of IOs performed
  long io_ops;

  struct event_base *base;
  redisAsyncContext *c;
};

//} // End namespace redis
