#include "Serializer.hpp"

#include <iostream>

using namespace std;
using namespace IODD;

int main() {
  cout << "Hello world" << endl;

  auto repo = deserializeModel("config");

  cout << "IODD Repository has " << repo.size() << " descriptors" << endl;

  exit(EXIT_SUCCESS);
}