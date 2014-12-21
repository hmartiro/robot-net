/**
* Example of Redis get/set usage for C++11.
*/

#include <iostream>
#include <chrono>
#include <hiredis/hiredis.h>

using namespace std;

static const string REDIS_HOST = "localhost";
static const int REDIS_PORT = 6379;

int main(int argc, char *argv[]) {

  redisContext *c;
  redisReply *reply;

  struct timeval timeout = { 1, 500000 }; // 1.5 seconds
  c = redisConnectWithTimeout(REDIS_HOST.c_str(), REDIS_PORT, timeout);
  if (c == NULL || c->err) {
    if (c) {
      printf("Connection error: %s\n", c->errstr);
      redisFree(c);
    } else {
      printf("Connection error: can't allocate redis context\n");
    }
    exit(1);
  }

  // Ping server
  reply = (redisReply*) redisCommand(c,"PING");
  printf("PING: %s\n", reply->str);
  freeReplyObject(reply);

  // Number of messages sent
  int count = 0;

  while(true) {

    // Get the key
    reply = static_cast<redisReply*>(redisCommand(c,"GET time"));
    count++;

    if(count % 10000 == 0) {
      unsigned long ms = std::chrono::system_clock::now().time_since_epoch() /
          std::chrono::milliseconds(1);
      cout << "[RECV] #" << count << " at " << ms << ": \"" << reply->str << "\"" << endl;
    }

    freeReplyObject(reply);
  }

  // Disconnect and free context
  redisFree(c);

  return 0;
}
