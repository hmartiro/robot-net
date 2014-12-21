/**
* Example of Redis get/set usage for C++11.
*/

#include <iostream>
#include <chrono>
#include <signal.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>
#include <thread>
#include <string.h>

using namespace std;

static const string REDIS_HOST = "localhost";
static const int REDIS_PORT = 6379;

void connectCallback(const redisAsyncContext *c, int status) {
  if (status != REDIS_OK) {
    printf("Error: %s\n", c->errstr);
    return;
  }
  printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status) {
  if (status != REDIS_OK) {
    printf("Error: %s\n", c->errstr);
    return;
  }
  printf("Disconnected...\n");
}

static int count = 0;
void onMessage(redisAsyncContext *c, void *reply, void *privdata) {

  redisReply *r = (redisReply*)reply;
  if (reply == NULL) return;

  count++;

  if(r->type != REDIS_REPLY_ARRAY) return;
  if(r->elements != 3) return;
  if(strcmp(r->element[0]->str, "message")) return;

  // Current time in ms
  unsigned long ms = chrono::system_clock::now().time_since_epoch() /
      chrono::milliseconds(1);

  if(count % 10000 == 0)
    cout << "[RECV] #" << count << " at " << ms << ": " << r->element[2]->str << endl;
}

int main(int argc, char *argv[]) {

  signal(SIGPIPE, SIG_IGN);
  struct event_base *base = event_base_new();

  redisAsyncContext *c = redisAsyncConnect(REDIS_HOST.c_str(), REDIS_PORT);
  if (c->err) {
    printf("Error: %s\n", c->errstr);
    return 1;
  }

  redisLibeventAttach(c, base);
  redisAsyncSetConnectCallback(c, connectCallback);
  redisAsyncSetDisconnectCallback(c, disconnectCallback);
  redisAsyncCommand(c, onMessage, NULL, "SUBSCRIBE time");

  event_base_dispatch(base);
  redisAsyncDisconnect(c);

  return 0;
}
