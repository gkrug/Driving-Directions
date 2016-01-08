#include <iostream>
#include <string>
#include "street_map.hpp"

using namespace std;

int main (int argc, char *argv[]) {
  street_map m("southbend.v2.map");
  string addr;
  int su, sv, tu, tv;
  float spos, tpos;
  bool found;

  found = false;
  while (!found && cin) {
    cout << "Starting address: ";
    getline(cin, addr);
    found = m.geocode(addr, su, sv, spos);
    if (!found)
      cout << "Address not found.\n";
  }

  found = false;
  while (!found && cin) {
    cout << "Ending address:   ";
    getline(cin, addr);
    found = m.geocode(addr, tu, tv, tpos);
    if (!found)
      cout << "Address not found.\n";
  }

  vector<pair<string,float>> steps;
  if (m.route(su, sv, spos, tu, tv, tpos, steps)) {
    for (auto step : steps)
      cout << step.second << " miles on " << step.first << endl;
  } else {
    cout << "Route not found.\n";
  }
}
