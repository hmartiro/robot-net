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
void getCallback(redisAsyncContext *c, void *r, void *privdata) {

  redisReply *reply = (redisReply*)r;
  if (reply == NULL) return;

  count++;

  // Current time in ms
  unsigned long ms = chrono::system_clock::now().time_since_epoch() /
      chrono::milliseconds(1);

  if(count % 10000 == 0)
    cout << "[GET] #" << count << " at " << ms << ": " << reply->str << endl;
}


void e_callback(evutil_socket_t fd, short what, void *arg) {

  // Get the key
  redisAsyncContext* c = (redisAsyncContext*)arg;
  redisAsyncCommand(c, getCallback, NULL, "GET time");
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

  struct timeval e_time = {0, 1};
  struct event* e = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, e_callback, c);
  event_add(e, &e_time);

  event_base_dispatch(base);
  redisAsyncDisconnect(c);

  return 0;
}
