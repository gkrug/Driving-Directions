#ifndef STREET_MAP_HPP
#define STREET_MAP_HPP

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <queue>

using namespace std;

// implement edge struct needed for adjacency list
struct edge {
	int stID;	// starting ID
	int endID;	// ending ID
	string streetName; // street name
	float length;
	float distance;
	// constructor
	edge(int sid, int eid, string name, float len) {
		stID = sid;
		endID = eid;
		streetName = name;
		length = len;
	}
	// less than operator compares lengths
	bool operator<(const edge &other) const {
		return (length < other.length);
	}
};
/*
struct gpair {
	pair <int, float> mypair;
	gpair( int i, float d ) { mypair = make_pair(i, d); }
	bool operator<(const gpair &other) const {
		return (mypair.second < other.mypair.second);
	}
	bool operator==(const gpair &other) const {
		return (mypair.first == other.mypair.first && mypair.second == other.mypair.second); 
	}
};
*/
struct graph {
	// in the map, the key is the initial node ID mapping to a pair
	// with first: distance; and second: vector of edges from the ID
	unordered_map<int, vector<edge>> edges;
	void insert(int idx, const edge &e) { edges[idx].push_back(e); }
};

// when processing nodes for dijkstra's algorithm, is the 
// element that stores the distance of each node and whether it's marked
struct distMarked {
	float fdistance;
	bool marked;
	std::vector<std::pair<std::string, float>> directions;
	distMarked(float f) { fdistance = f; marked = false; }
};

struct mycompare {
	bool operator()(const pair<int, float> &x, const pair<int, float> &y) {
		return (x.second > y.second);
	}
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

struct side {
	std::string street_name;
	int parity;
	bool operator==(const side &other ) const {
		return (street_name == other.street_name && parity == other.parity);
	}
};

namespace std {
  template<>
  struct hash<side> {
      std::size_t operator() (side const& s) const {
      std::size_t h = std::hash<string>()(s.street_name);
      hash_combine(h, s.street_name);
      return h;
    }
  };
}

struct segment {
	int startingNum;
	int endingNum;
	int NodeStart;
	int NodeEnd;
	float SegmentLength;
	std::string URL;
};


class street_map {
public:
  // Constructor.
  // `filename` is the pathname of a file with the format described in pg4.pdf.

  explicit street_map (const std::string &filename);

  // Geocodes an address.
  // Input argument:
  //   `address` is an address of the form "1417 E Wayne St"
  // Return value:
  //   `true` if address is found, `false` if address is not found
  // Output arguments:
  //   `u` is the starting node of the edge on which the location lies
  //   `v` is the ending node of the edge
  //   `pos` is how far along the edge the location lies

  bool geocode(const std::string &address, int &u, int &v, float &pos) const;

  // Three functions for finding the shortest route.

  // Input arguments:
  //   `source` is the source node
  //   `target` is the target node
  // Output argument:
  //   `distance` is the shortest distance between them

  bool route3(int source, int target, float &distance);

  // Input arguments:
  //   `su`, `sv` is the edge on which the source lies
  //   `spos`     is how far along the edge the source lies
  //   `tu`, `tv` is the edge on which the source lies
  //   `tpos`     is how far along the edge the source lies
  // Output argument:
  //   `distance` is the shortest distance between them

  bool route4(int su, int sv, float spos, int tu, int tv, float tpos, float &distance);

  // Input arguments:
  //   `su`, `sv` is the edge on which the source lies
  //   `spos`     is how far along the edge the source lies
  //   `tu`, `tv` is the edge on which the source lies
  //   `tpos`     is how far along the edge the source lies
  // Output argument:
  //   `steps`    is a sequence of driving directions;
  //              each member is a canonical street name and a distance.
  //              The original contents (if any) are cleared out.

  bool route(int su, int sv, float spos, int tu, int tv, float tpos, std::vector<std::pair<std::string, float>> &steps);
  bool directions(int source, int target, float &distance,vector<pair<string, float>> &steps);
  void reinitialize();
  unordered_map<side, vector<segment>> strMap; 

  priority_queue<pair<int, float>, vector<pair<int,float>>, mycompare>  minqu;

  graph g;

  unordered_map<int, distMarked> checker;  

};

#endif
