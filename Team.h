#ifndef TEAM_H
#define TEAM_H

#include <iostream>
#include <string>
#include <vector>

struct City
{
    std::string name;
    double latitude;
    double longitude;

    City() : name(""), latitude(0.0), longitude(0.0) {}
    City(std::string cityName, double lat, double lon)
        : name(std::move(cityName)), latitude(lat), longitude(lon) {}
};

class Team
{
public:
    Team();
    Team(std::string teamName, std::string abbreviation, std::string teamColor,
         double eloRating, std::string cityName, double lat, double lon, int schedule);
    ~Team();

    // Getter functions
    std::string getName() const;
    std::string getAbbreviation() const;
    std::string getColor() const;
    double getElo() const;
    const City &getCity() const;
    int getSchedule() const;

private:
    std::string name;
    std::string abbrev;
    std::string color;
    double elo;
    City city;
    int scheduleIdx;
};

#endif // TEAM_H
