#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <cmath>

struct Stop {
    std::string stop_id;
    std::string stop_name;
    double lat;
    double lon;
    int wheelchair_boarding = 1; // 1 = accessible, 0 = not accessible
};

struct Route {
    std::string route_id;
    std::string route_short_name;
    std::string route_long_name;
    int route_type; // 1 = Subway, 3 = Bus
    std::string route_color;
};

struct Trip {
    std::string route_id;
    std::string trip_id;
    std::string trip_headsign;
};

struct StopTime {
    std::string trip_id;
    int arrival_time;
    int departure_time;
    std::string stop_id;
    int stop_sequence;
};

struct LegInfo {
    std::string mode;
    std::string line;
    std::string color;
    std::string direction;
    std::string fromStop;
    std::string toStop;
    int boardMin;
    int alightMin;
    int fare;
    double walkDistanceM;
    double distance_km; // For CO2 calc
    std::vector<Stop> intermediateStops;
    std::vector<std::pair<double, double>> path;
};

struct RouteState {
    int time_mins = 999999;
    int fare_inr = 999999;
    int transfers = 999999;
    double walk_effort = 999999;
    double co2_saved_grams = 0.0;
    double crowd_penalty = 0.0;
    std::vector<LegInfo> legs;
    
    bool dominates(const RouteState& other) const {
        bool at_least = (time_mins <= other.time_mins) && (fare_inr <= other.fare_inr) && (transfers <= other.transfers) && (walk_effort <= other.walk_effort);
        bool strictly = (time_mins < other.time_mins) || (fare_inr < other.fare_inr) || (transfers < other.transfers) || (walk_effort < other.walk_effort);
        return at_least && strictly;
    }
};