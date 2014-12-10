/**
* Example of basic msgpack usage for C++11.
*/

#include <vector>
#include <string>
#include <iostream>
#include <msgpack.hpp>

using namespace std;

int main() {

  // This is the target object.
  vector<string> target;
  target.push_back("Hello,");
  target.push_back("World!");

  // Serialize it.
  msgpack::sbuffer sbuf;  // simple buffer
  msgpack::pack(&sbuf, target);

  // Deserialize the serialized data.
  msgpack::unpacked msg;    // includes memory pool and deserialized object
  msgpack::unpack(&msg, sbuf.data(), sbuf.size());
  msgpack::object obj = msg.get();

  // Print the deserialized object to stdout.
  cout << obj << endl;    // ["Hello," "World!"]

  // Convert the deserialized object to staticaly typed object.
  vector<string> result;
  obj.convert(&result);

  // If the type is mismatched, it throws msgpack::type_error.
  // obj.as<int>();  // type is mismatched, msgpack::type_error is thrown

  return 0;
}
