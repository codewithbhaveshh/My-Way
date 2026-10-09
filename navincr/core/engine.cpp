#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>
#include "graph_loader.h"

using namespace std;

struct Label {
    int node;
    double time;
    double cost;
    int transfers;
    double walkM;
    double co2;
    string mode;
    vector<int> path;
    bool dominates(const Label& o) const {
        return time <= o.time && cost <= o.cost && transfers <= o.transfers 
            && walkM <= o.walkM && co2 >= o.co2
            && (time < o.time || cost < o.cost || transfers < o.transfers 
                || walkM < o.walkM || co2 > o.co2);
    }
};

struct CompareLabel {
    bool operator()(const Label& a, const Label& b) {
        return a.time > b.time;
    }
};

class ParetoEngine {
public:
    Graph graph;
    bool loadGraph(const string& path) {
        return graph.loadFromJSON(path);
    }
    vector<Label> findRoutes(const string& originId, const string& destId, int departureMin = 480) {
        int origin = graph.getNodeIndex(originId);
        int dest = graph.getNodeIndex(destId);
        if (origin < 0 || dest < 0) return {};
        unordered_map<int, vector<Label>> paretoSets;
        priority_queue<Label, vector<Label>, CompareLabel> pq;
        Label startLabel;
        startLabel.node = origin;
        startLabel.time = departureMin;
        startLabel.cost = 0;
        startLabel.transfers = 0;
        startLabel.walkM = 0;
        startLabel.co2 = 0;
        startLabel.mode = "start";
        startLabel.path = {origin};
        pq.push(startLabel);
        vector<Label> results;
        while (!pq.empty()) {
            Label cur = pq.top();
            pq.pop();
            bool dominated = false;
            for (auto& existing : paretoSets[cur.node]) {
                if (existing.dominates(cur)) { dominated = true; break; }
            }
            if (dominated) continue;
            auto& pset = paretoSets[cur.node];
            pset.erase(remove_if(pset.begin(), pset.end(), [&](const Label& l) {
                return cur.dominates(l);
            }), pset.end());
            pset.push_back(cur);
            if (cur.node == dest) {
                results.push_back(cur);
                if (results.size() >= 5) break;
                continue;
            }
            for (auto& edge : graph.getEdges(cur.node)) {
                Label next;
                next.node = edge.to;
                next.time = cur.time + edge.travelTimeMin;
                next.cost = cur.cost + edge.fareINR;
                next.transfers = cur.transfers + (edge.mode != cur.mode && cur.mode != "start" ? 1 : 0);
                next.walkM = cur.walkM + (edge.mode == "walk" ? edge.distanceM : 0);
                next.co2 = cur.co2 + edge.co2SavedG;
                next.mode = edge.mode;
                next.path = cur.path;
                next.path.push_back(edge.to);
                pq.push(next);
            }
        }
        return results;
    }
    string toJSON(const vector<Label>& routes) {
        ostringstream out;
        out << "{\"routes\":[";
        for (size_t i = 0; i < routes.size(); i++) {
            auto& r = routes[i];
            out << "{";
            out << "\"label\":\"Route " << (i+1) << "\",";
            out << "\"totalDurationMin\":" << (r.time - 480) << ",";
            out << "\"totalFare\":" << r.cost << ",";
            out << "\"transfers\":" << r.transfers << ",";
            out << "\"walkPenaltyM\":" << r.walkM << ",";
            out << "\"co2SavedGrams\":" << r.co2 << ",";
            out << "\"legs\":[";
            for (size_t j = 0; j + 1 < r.path.size(); j++) {
                auto& node = graph.nodes[r.path[j]];
                auto& nextNode = graph.nodes[r.path[j+1]];
                if (j > 0) out << ",";
                out << "{";
                out << "\"fromStop\":\"" << node.name << "\",";
                out << "\"toStop\":\"" << nextNode.name << "\",";
                out << "\"path\":[[" << node.lat << "," << node.lon << "],";
                out << "[" << nextNode.lat << "," << nextNode.lon << "]]";
                out << "}";
            }
            out << "]";
            out << "}";
            if (i + 1 < routes.size()) out << ",";
        }
        out << "]}";
        return out.str();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 5) return 1;
    double startLat = atof(argv[1]);
    double startLon = atof(argv[2]);
    double endLat = atof(argv[3]);
    double endLon = atof(argv[4]);
    int departure = argc >= 6 ? atoi(argv[5]) : 480;
    ParetoEngine engine;
    if (!engine.loadGraph("data/delhi_ncr.json")) return 1;
    string origin = engine.graph.findNearestNode(startLat, startLon);
    string dest = engine.graph.findNearestNode(endLat, endLon);
    if (origin.empty() || dest.empty()) return 1;
    auto routes = engine.findRoutes(origin, dest, departure);
    cout << engine.toJSON(routes) << endl;
    return 0;
}
