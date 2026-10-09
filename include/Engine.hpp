#pragma once
#include "GTFSData.hpp"
#include <vector>
#include <unordered_map>
#include <string>

class ParetoEngine {
private:
    std::unordered_map<std::string, Stop> stops;
    std::unordered_map<std::string, Route> routes;
    std::unordered_map<std::string, Trip> trips;
    std::vector<StopTime> stop_times;
    
    std::unordered_map<std::string, std::vector<RouteState>> pareto_fronts;
    
    double calculateWalkPenalty(int current_time_mins, double distance_km, double temp, int weather_code);
    double haversineM(double lat1, double lon1, double lat2, double lon2);

    double current_temp = 25.0;
    int current_weather_code = 0;
    double traffic_multiplier = 1.0;
    double current_aqi = 50.0;
    bool require_wheelchair = false;
    std::unordered_map<std::string, std::string> crowd_data;
    
    // Avoid Polygons (OpenRouteService)
    struct Polygon { std::vector<std::pair<double, double>> points; };
    std::vector<Polygon> avoid_polygons;
    
    // Check if a point is inside an avoid polygon using ray casting
    bool isPointInAvoidPolygon(double lat, double lon);

public:
    ParetoEngine(
        std::unordered_map<std::string, Stop> s, 
        std::unordered_map<std::string, Route> r,
        std::unordered_map<std::string, Trip> t,
        std::vector<StopTime> st);
        
    void setConditions(double temp, int weather_code, double traffic_mult, double aqi, bool wheelchair, const std::unordered_map<std::string, std::string>& crowd, const std::vector<Polygon>& polygons = {}) {
        current_temp = temp;
        current_weather_code = weather_code;
        traffic_multiplier = traffic_mult;
        current_aqi = aqi;
        require_wheelchair = wheelchair;
        crowd_data = crowd;
        avoid_polygons = polygons;
    }

    bool addStateIfOptimal(const std::string& stop_id, const RouteState& new_state);
    void runParetoSearch(const std::string& source_stop, const std::string& dest_stop, int start_time_mins);
    std::vector<RouteState> getParetoFront(const std::string& dest_stop);
};