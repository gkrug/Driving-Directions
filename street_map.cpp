#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <climits>
#include <cmath>

#include "street_map.hpp"

#define INF 1000000
using namespace std;

street_map::street_map (const string &filename) {
ifstream fp(filename);	// open file
	cout << "opening file " << filename << endl;
	string temp_str, url, name;
	int par, start, end;
	int st_node, end_node;
	float segment_length;
	double temp_num;
	char c;
	side sid_odd, sid_even;
	segment seg;
	vector<segment> even_segs;
	vector<segment> odd_segs;
	vector<edge> egs;
	fp >> temp_str;
	while ( !fp.eof() ) {
		// read in if line contains name of a street
		//cout << temp_str << endl;
		if (temp_str == "N:") {
			getline(fp, temp_str);
			//cout << temp_str << endl;
			temp_str.erase(0, 1);
			//cout << temp_str << endl;
			sid_even.street_name = sid_odd.street_name = name = temp_str;
			sid_even.parity = 0; sid_odd.parity = 1;
			fp >> temp_str; // get N: or R: tag for next loop
			//cout << "here\n";
		} else if (temp_str == "R:") {
			fp >> par >> start >> end >> st_node >> end_node >> segment_length;
			getline(fp, url);
			url.erase(0, 1);						
			//cout << st_node << end_node << endl;						
			// fill segment struct
			seg.startingNum = start;
			seg.endingNum = end;
			seg.NodeStart = st_node;
			seg.NodeEnd = end_node;
			seg.SegmentLength = segment_length;
			seg.URL = url;

			// ********** changes for PG5 ****************

			// fill edge
			edge e(st_node, end_node, name, segment_length);
			edge eb(end_node,st_node, name, segment_length);
			// insert edge in map
			g.insert(st_node, e); 			
			g.insert(end_node, eb);
			// update processing/marked vector container
			distMarked chk1(INF);
			checker.insert(pair<int,distMarked>(e.stID, chk1)); 
			checker.insert(pair<int,distMarked>(eb.stID, chk1)); 
			//if (e.stID == 11395595) { cout << "insert here" << checker.at(e.stID).marked << endl;}

			// initialize priority queue
			minqu.push(make_pair(e.stID, INF));
			minqu.push(make_pair(e.endID, INF));

			// **************************************
						
			// push segment into vector
			(par == 0) ? even_segs.push_back(seg) : odd_segs.push_back(seg);


			// get N: or R: tag for next loop. If N: then insert vec in map
			temp_str.clear();
				if (!fp.eof()) fp >> temp_str;
				else 				break;
								
			if (temp_str == "N:") {
				strMap.insert(pair<side, vector<segment> >(sid_even, even_segs)); //cout << sid_even.street_name << " " << sid_even.parity << endl;
				strMap.insert(pair<side, vector<segment> > (sid_odd, odd_segs));
	
				even_segs.clear();
				odd_segs.clear();
			}
			
		} else if (temp_str == "E:") {
				//getline(fp, temp_str);
				fp >> seg.NodeStart >> seg.NodeEnd >> seg.SegmentLength;
				getline(fp, seg.URL);
				seg.URL.erase(0, 1);
				// ********** changes for PG5 ****************
				// fill edge
				edge e1(seg.NodeStart, seg.NodeEnd, name, seg.SegmentLength);
				edge e2(seg.NodeEnd, seg.NodeStart, name, seg.SegmentLength);
				// insert edge into map
				g.insert(e1.stID, e1);
				g.insert(e2.stID, e2);
				//cout << "here11\n";
				// update processing/marked vector
				distMarked chk(INF);
				checker.insert(pair<int,distMarked>(e1.stID, chk));
				checker.insert(pair<int,distMarked>(e2.stID, chk));
				
				// initialize priority queue
				minqu.push(make_pair(e1.stID, INF));
				minqu.push(make_pair(e1.endID, INF));
				// ********************************************
				//cout << "here3\n";
				fp >> temp_str;
		} else {
			cerr << "must begin with N: or R: " << endl;
			//cout << "arg is " << temp_str << endl;
			exit(1);
		}
		//cout << temp_str << endl;
	}
}

bool street_map::geocode(const string &address, int &u, int &v, float &pos) const {
stringstream ss(address);

	int num;
	string addr;
	side s;
	
	// parse the string to get addr number and then street name
	ss >> num;
	getline(ss, addr);
	addr.erase(0,1);
	s.street_name = addr;
	(num % 2 == 0) ? s.parity = 0 : s.parity = 1;
	

	// search the map  structure to see if input is there

	// map iterator equal to the result of the find key function
	//unordered_map<side, vector<segment> > iterator it = strMap.find(s);
   //cout << "searching for \n" << addr << "\n";
	if (strMap.find(s) == strMap.end()) {
		//cout << "*********not found\n";
		return false;
	} 
		
	for (segment i : strMap.at(s)) {
		if (num >= i.startingNum && num <= i.endingNum) {
			//cout << "\n*****\naddr is" << num << " start is " << i.startingNum << " end is " << i.endingNum << " url is " << i.URL << "** ***\n\n" <<endl;
			u = i.NodeStart;
			v = i.NodeEnd;
			pos = (i.SegmentLength) * ( (float) (num - i.startingNum) / (float) (i.endingNum - i.startingNum + 2) );
			//url = i.URL;
			return true;
		}
	}

	return false;	
}

// SEGFAULT OCCURING WHEN THIS IS CALLED
bool street_map::route3(int source, int target, float &distance) {
   if (checker.find(source) == checker.end() || checker.find(target) == checker.end()) {
			cout << "returning false\n";
			return false;
		}

	minqu.push(make_pair(source, 0));
	//cout << "initial dist is " << checker.at(target).fdistance << endl;
	while (!minqu.empty()) {

		pair<int, float> current = minqu.top();
		minqu.pop();
		int sid = current.first;
		float dist = current.second;
		if (checker.at(sid).marked) continue;

		//cout << "here: sid is " << sid << endl;
		checker.at(sid).marked = true;

		// iterate through adjacent nodes
		for (edge i : g.edges.at(sid)) {		
			if (i.endID == target) {
				//cout << "comparing to target\n";
				//cout << "\nchecking " << i.stID << " to " << i.endID << " poss len " << i.length << " + " << dist << " <? " << checker.at(i.endID).fdistance << endl;
			}
			if (/*!checker.at(i.endID).marked && */i.length + dist < checker.at(i.endID).fdistance) {
				minqu.push(make_pair(i.endID, i.length + dist));
				checker.at(i.endID).fdistance = i.length + dist;
			}
		}
	}
	
	// output distance
	distance = checker.at(target).fdistance;
	reinitialize();
	if (distance != INF)
		return true;
	else 
		return false;

}

bool street_map::route4(int su, int sv, float spos, int tu, int tv, float tpos, float &distance) {
	// create fake node for start	
	static int fkid1 = -1;
	static int fkid2 = -2;
	float tmplen;
	distMarked chk(INF);
	string n1, n2;
	// create fake ids for the starting node to specified address
	for (auto i : g.edges.at(su)) {
		if (i.endID == sv) {
			tmplen = i.length;
			n1 = i.streetName;
			edge temp1(fkid1, su, i.streetName, spos);
			edge temp3(su, fkid1, i.streetName, spos);
			g.insert(fkid1, temp1);
			g.insert(su, temp3);
			break;
		}
	}
	for (auto i : g.edges.at(sv)) {
		if (i.endID == su) {
			float l = i.length;
			edge temp2(fkid1, sv, i.streetName, l - spos);
			edge temp4(sv, fkid1, i.streetName, l - spos);
			g.insert(fkid1, temp2);
			g.insert(sv, temp4);
			break;
		}
	}
	// create fake ids from specified address to ending id
	for (auto i : g.edges.at(tu)) {
		if (i.endID == tv) {
			n2 = i.streetName;
			edge temp1(fkid2, tu, i.streetName, tpos);
			edge temp3(tu, fkid2, i.streetName, tpos);
			g.insert(fkid2, temp1);
			g.insert(tu, temp3);
			break;
		}
	}
	for (auto i : g.edges.at(tv)) {
		if (i.endID == tu) {
			float l = i.length;
			//i.endID = fkid2;
			//i.length = tmplen - tpos;
			edge temp2(fkid2, tv, i.streetName, l - tpos);
			edge temp4(tv, fkid2, i.streetName, l - tpos);
			g.insert(fkid2, temp2);
			g.insert(tv, temp4);
			break;
		}
	}

	// initialize first fake node
	checker.insert(pair<int, distMarked>(fkid1, chk));
	minqu.push(make_pair(fkid1, INF));

	// initialize second fake node
	checker.insert(pair<int, distMarked>(fkid2, chk));
	minqu.push(make_pair(fkid2, INF));
	
	// if on the same segment, return difference, else use dijstra's algorithm with fake nodes
	if (su == tu) { distance = tpos - spos; fkid1 -= 2; fkid2 -= 2; return true;}

	bool result = route3(fkid1, fkid2, distance);
	fkid1 -= 2; fkid2 -= 2;
	//if (su == tu) { distance = tpos - spos; result = true; }

	return result;
}

bool street_map::route(int su, int sv, float spos, int tu, int tv, float tpos, vector<pair<string, float>> &steps) {
  steps.clear();
  // create fake node for start	
	static int fkid1 = -1;
	static int fkid2 = -2;
	float tmplen;
	float distance;
	distMarked chk(INF);
	string n1, n2;
	// create fake ids for the starting node to specified address
	for (auto i : g.edges.at(su)) {
		if (i.endID == sv) {
			tmplen = i.length;
			n1 = i.streetName;
			edge temp1(fkid1, su, i.streetName, spos);
			edge temp3(su, fkid1, i.streetName, spos);
			g.insert(fkid1, temp1);
			g.insert(su, temp3);
			break;
		}
	}
	for (auto i : g.edges.at(sv)) {
		if (i.endID == su) {
			float l = i.length;
			edge temp2(fkid1, sv, i.streetName, l - spos);
			edge temp4(sv, fkid1, i.streetName, l - spos);
			g.insert(fkid1, temp2);
			g.insert(sv, temp4);
			break;
		}
	}
	// create fake ids from specified address to ending id
	for (auto i : g.edges.at(tu)) {
		if (i.endID == tv) {
			n2 = i.streetName;
			edge temp1(fkid2, tu, i.streetName, tpos);
			edge temp3(tu, fkid2, i.streetName, tpos);
			g.insert(fkid2, temp1);
			g.insert(tu, temp3);
			break;
		}
	}
	for (auto i : g.edges.at(tv)) {
		if (i.endID == tu) {
			float l = i.length;
			//i.endID = fkid2;
			//i.length = tmplen - tpos;
			edge temp2(fkid2, tv, i.streetName, l - tpos);
			edge temp4(tv, fkid2, i.streetName, l - tpos);
			g.insert(fkid2, temp2);
			g.insert(tv, temp4);
			break;
		}
	}

	// initialize first fake node
	checker.insert(pair<int, distMarked>(fkid1, chk));
	minqu.push(make_pair(fkid1, INF));

	// initialize second fake node
	checker.insert(pair<int, distMarked>(fkid2, chk));
	minqu.push(make_pair(fkid2, INF));
	
	// if on the same segment, return difference, else use dijstra's algorithm with fake nodes
	if (su == tu) { 
		distance = tpos - spos; 
		fkid1 -= 2; fkid2 -= 2; 
		steps.push_back(make_pair(n1, distance));
		return true;
	}

	bool result = directions(fkid1, fkid2, distance, steps);
	fkid1 -= 2; fkid2 -= 2;
	//if (su == tu) { distance = tpos - spos; result = true; }

	return result;
}

bool street_map::directions(int source, int target, float &distance,vector<pair<string, float>> &steps) {
	if (checker.find(source) == checker.end() || checker.find(target) == checker.end()) {
			cout << "returning false\n";
			return false;
		}

	minqu.push(make_pair(source, 0));
	//cout << "initial dist is " << checker.at(target).fdistance << endl;
	while (!minqu.empty()) {

		pair<int, float> current = minqu.top();
		minqu.pop();
		int sid = current.first;
		float dist = current.second;
		if (checker.at(sid).marked) continue;

		//cout << "here: sid is " << sid << endl;
		checker.at(sid).marked = true;

		// iterate through adjacent nodes
		for (edge i : g.edges.at(sid)) {		
			if (i.endID == target) {
				//cout << "comparing to target\n";
				//cout << "\nchecking " << i.stID << " to " << i.endID << " poss len " << i.length << " + " << dist << " <? " << checker.at(i.endID).fdistance << endl;
			}
			if (/*!checker.at(i.endID).marked && */i.length + dist < checker.at(i.endID).fdistance) {
				minqu.push(make_pair(i.endID, i.length + dist));
				checker.at(i.endID).fdistance = i.length + dist;
				if (!checker.at(i.stID).directions.empty()) {
					if (checker.at(i.stID).directions.back().first == i.streetName) {
						checker.at(i.endID).directions = checker.at(i.stID).directions;
						checker.at(i.endID).directions.back().second += i.length;
					} else {
						checker.at(i.endID).directions = checker.at(i.stID).directions;
						checker.at(i.endID).directions.push_back(make_pair(i.streetName, i.length));
					}
				}
				else {
					checker.at(i.endID).directions = checker.at(i.stID).directions;
					checker.at(i.endID).directions.push_back(make_pair(i.streetName, i.length));
				}
			}
		}
	}
	
	// output distance
	distance = checker.at(target).fdistance;

	// copy final steps from checker data structure
	steps = checker.at(target).directions;

	// print out the steps
	//for (auto i : steps) cout << i.first << " " << i.second << endl;
	reinitialize();
	if (distance != INF)
		return true;
	else 
		return false;
}

void street_map::reinitialize() {
	for (auto i : g.edges) {
		checker.at(i.first).marked = false;
		checker.at(i.first).fdistance = INF;
		minqu.push(make_pair(i.first, INF));
	}
}
