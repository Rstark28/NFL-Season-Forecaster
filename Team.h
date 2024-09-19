#ifndef TEAM_H
#define TEAM_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory> // for std::shared_ptr

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
    float getWinCount() const;
    bool isPlayoffTeam() const;
    int getPlayoffRound() const;

    // Setter functions
    void updateElo(double eloChange);
    void updateWinCount(float result);
    void setPlayoffTeam(bool madePlayoffs);
    void setPlayoffRound(int round);

    // New functions for losses tracking
    void addLoss(const std::shared_ptr<Team> &opponent, int pointDifferential);
    const std::map<std::shared_ptr<Team>, int> &getTeamsLostTo() const;

private:
    std::string name;
    std::string abbrev;
    std::string color;
    double elo;
    City city;
    int scheduleIdx;
    float winCount;
    bool playoffTeam; // Variable to track if the team made the playoffs
    int playoffRound; // Variable to track the round the team made it to

    // Map to store losses with point differentials
    std::map<std::shared_ptr<Team>, int> teamsLostTo;
};

#endif // TEAM_H
