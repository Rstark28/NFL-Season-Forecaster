#ifndef TEAM_H
#define TEAM_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

// Struct to represent a city with a name and geographical coordinates
struct City
{
    std::string name;
    double latitude;
    double longitude;

    City() : name(""), latitude(0.0), longitude(0.0) {}
    City(std::string cityName, double lat, double lon)
        : name(std::move(cityName)), latitude(lat), longitude(lon) {}
};

// Class to represent a team with various attributes and functionalities
class Team
{
public:
    // Constructors and Destructor
    Team();
    Team(std::string teamName, std::string abbreviation, std::string teamColor,
         double eloRating, std::string cityName, double lat, double lon, int scheduleIndex);
    ~Team();

    // Getter functions
    std::string getName() const;
    std::string getAbbreviation() const;
    std::string getColor() const;
    double getEloRating() const;
    const City &getCity() const;
    int getScheduleIndex() const;
    float getWinCount() const;
    bool hasMadePlayoffs() const;
    int getPlayoffRound() const;

    // Setter functions
    void updateEloRating(double eloChange);
    void updateWinCount(float result);
    void setPlayoffStatus(bool madePlayoffs);
    void setPlayoffRound(int round);
    void resetTeam();

    // Functions for tracking losses
    void addLoss(const std::shared_ptr<Team> &opponent, int pointDifferential);
    const std::map<std::shared_ptr<Team>, int> &getLosses() const;

private:
    // Private member variables
    std::string name;         // Team name
    std::string abbreviation; // Team abbreviation
    std::string color;        // Team color
    double eloRating;         // Team's Elo rating
    double orgEloRating;      // Original Elo rating
    City city;                // City where the team is based
    int scheduleIndex;        // Index in the schedule
    float winCount;           // Number of wins
    bool playoffStatus;       // Whether the team made the playoffs
    int playoffRound;         // The playoff round the team reached

    // Map to store losses with point differentials
    std::map<std::shared_ptr<Team>, int> losses;
};

#endif // TEAM_H
