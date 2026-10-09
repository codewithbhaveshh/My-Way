#pragma once
#include "GTFSData.hpp"
#include <string>
#include <vector>

class GTFSParser {
public:
    static std::unordered_map<std::string, Stop> parseStops(const std::string& filepath);
    static std::vector<StopTime> parseStopTimes(const std::string& filepath);
    static std::unordered_map<std::string, Route> parseRoutes(const std::string& filepath);
    static std::unordered_map<std::string, Trip> parseTrips(const std::string& filepath);
    static std::unordered_map<std::string, std::string> parseCrowdData(const std::string& filepath);
    static int timeToMinutes(const std::string& timeStr);
};