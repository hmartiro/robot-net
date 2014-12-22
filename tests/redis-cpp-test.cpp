/**
 * Test of the Redis C++ wrapper.
 */

#include <iostream>
#include <thread>
#include "../src/redis.hpp"

using namespace std;

static const string REDIS_HOST = "localhost";
static const int REDIS_PORT = 6379;

int main(int argc, char* argv[]) {

  Redis r(REDIS_HOST, REDIS_PORT);

//  r.set("name", "lol");
//  r.set("blah", "whatup!!");
//  r.get("blah", [](string key, string value) {
//    cout << "[GET] " << key << ": " << value << endl;
//  });

  for(int i = 0; i < 1000000; i++) {
    r.command_async("set count " + to_string(i), [](string cmd, redisReply *reply) {
      if(!cmd.compare("set count 100000"))
        cout << cmd << ": " << reply->str << endl;
    });
  }

  thread loop([&r] { r.start(); });
  loop.join();

  return 0;
}
