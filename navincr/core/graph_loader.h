#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

struct Node {
    std::string osmId;
    std::string name;
    double lat;
    double lon;
};

struct Edge {
    int to;
    double travelTimeMin;
    double fareINR;
    double distanceM;
    double co2SavedG;
    std::string mode;
    std::string line;
    std::string color;
};

class Graph {
public:
    std::vector<Node> nodes;
    std::unordered_map<std::string, int> nodeIndex;
    std::vector<std::vector<Edge>> adjList;

    int getNodeIndex(const std::string& id) {
        auto it = nodeIndex.find(id);
        return it != nodeIndex.end() ? it->second : -1;
    }

    const std::vector<Edge>& getEdges(int idx) {
        return adjList[idx];
    }

    static double haversine(double lat1, double lon1, double lat2, double lon2) {
        double dLat = (lat2 - lat1) * 3.14159265358979323846 / 180.0;
        double dLon = (lon2 - lon1) * 3.14159265358979323846 / 180.0;
        lat1 = lat1 * 3.14159265358979323846 / 180.0;
        lat2 = lat2 * 3.14159265358979323846 / 180.0;
        double a = sin(dLat / 2) * sin(dLat / 2) + sin(dLon / 2) * sin(dLon / 2) * cos(lat1) * cos(lat2);
        return 6371000.0 * 2 * atan2(sqrt(a), sqrt(1 - a));
    }

    std::string findNearestNode(double lat, double lon) {
        double minDist = 1e18;
        std::string bestId = "";
        for (const auto& n : nodes) {
            double d = haversine(lat, lon, n.lat, n.lon);
            if (d < minDist) {
                minDist = d;
                bestId = n.osmId;
            }
        }
        return bestId;
    }

    bool loadFromJSON(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return false;
        std::stringstream buf;
        buf << file.rdbuf();
        std::string json = buf.str();
        size_t nStart = json.find("\"nodes\"");
        if (nStart == std::string::npos) return false;
        size_t noStart = json.find('{', nStart + 7);
        size_t noEnd = matchBracket(json, noStart);
        parseNodes(json, noStart, noEnd);
        adjList.resize(nodes.size());
        size_t eStart = json.find("\"edges\"");
        if (eStart == std::string::npos) return false;
        size_t eoStart = json.find('[', eStart);
        size_t eoEnd = matchBracket(json, eoStart);
        parseEdges(json, eoStart, eoEnd);
        return true;
    }

private:
    size_t matchBracket(const std::string& s, size_t pos) {
        char open = s[pos];
        char close = (open == '[') ? ']' : '}';
        int depth = 1;
        bool inStr = false;
        for (size_t i = pos + 1; i < s.size(); i++) {
            if (s[i] == '"' && (i == 0 || s[i - 1] != '\\')) inStr = !inStr;
            else if (!inStr) {
                if (s[i] == open) depth++;
                else if (s[i] == close && --depth == 0) return i;
            }
        }
        return std::string::npos;
    }

    double getNum(const std::string& json, size_t start, size_t end, const std::string& key) {
        size_t p = json.find("\"" + key + "\"", start);
        if (p == std::string::npos || p > end) return 0;
        p = json.find(':', p);
        if (p == std::string::npos || p > end) return 0;
        p++;
        while (p < end && (json[p] == ' ' || json[p] == '\t' || json[p] == '\n' || json[p] == '\r')) p++;
        size_t q = p;
        while (q < end && (isdigit(json[q]) || json[q] == '.' || json[q] == '-' || json[q] == 'e' || json[q] == 'E' || json[q] == '+')) q++;
        return (q > p) ? std::stod(json.substr(p, q - p)) : 0;
    }

    std::string getIdStr(const std::string& json, size_t start, size_t end, const std::string& key) {
        size_t p = json.find("\"" + key + "\"", start);
        if (p == std::string::npos || p > end) return "";
        p = json.find(':', p);
        if (p == std::string::npos || p > end) return "";
        p++;
        while (p < end && (json[p] == ' ' || json[p] == '\t' || json[p] == '\n' || json[p] == '\r')) p++;
        size_t q = p;
        while (q < end && (isdigit(json[q]) || json[q] == '-')) q++;
        return json.substr(p, q - p);
    }

    void parseNodes(const std::string& json, size_t start, size_t end) {
        size_t p = start + 1;
        while (p < end) {
            size_t k1 = json.find('"', p);
            if (k1 == std::string::npos || k1 >= end) break;
            size_t k2 = json.find('"', k1 + 1);
            if (k2 == std::string::npos || k2 >= end) break;
            std::string osmId = json.substr(k1 + 1, k2 - k1 - 1);
            size_t v1 = json.find('{', k2);
            if (v1 == std::string::npos || v1 >= end) break;
            size_t v2 = matchBracket(json, v1);
            if (v2 == std::string::npos || v2 > end) break;
            Node n;
            n.osmId = osmId;
            n.name = osmId;
            n.lat = getNum(json, v1, v2, "lat");
            n.lon = getNum(json, v1, v2, "lon");
            nodeIndex[osmId] = nodes.size();
            nodes.push_back(n);
            p = v2 + 1;
        }
    }

    void parseEdges(const std::string& json, size_t start, size_t end) {
        size_t p = start + 1;
        while (p < end) {
            size_t o1 = json.find('{', p);
            if (o1 == std::string::npos || o1 >= end) break;
            size_t o2 = matchBracket(json, o1);
            if (o2 == std::string::npos || o2 > end) break;
            int u = getNodeIndex(getIdStr(json, o1, o2, "u"));
            int v = getNodeIndex(getIdStr(json, o1, o2, "v"));
            if (u >= 0 && v >= 0) {
                double len = getNum(json, o1, o2, "length");
                double spd = getNum(json, o1, o2, "speed");
                if (spd <= 0) spd = 30.0;
                Edge e;
                e.to = v;
                e.distanceM = len;
                e.travelTimeMin = (len / 1000.0) / spd * 60.0;
                e.fareINR = 0;
                e.co2SavedG = 0;
                e.mode = "drive";
                e.line = "";
                e.color = "#4285F4";
                adjList[u].push_back(e);
            }
            p = o2 + 1;
        }
    }
};
