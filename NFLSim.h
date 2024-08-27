#ifndef NFLSIM_H
#define NFLSIM_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <cmath>

#include "Game.h"

class NFLSim
{
public:
    NFLSim(const std::string &filename);
    ~NFLSim();

    void readSchedule(const std::string &filename);
    std::vector<std::string> processGameInfo(std::string teamName, std::string gameInfo, int week);
    void readTeams(const std::string &filename);
    void printSchedule() const;
    void getHomeOddsStandard(const Game &game);

private:
    double calculateDistance(const City &homeCity, const City &awayCity);
    double adjustEloForByes(const Game &game, const Team &homeTeam, const Team &awayTeam);
    double calculateHomeOdds(double elo_diff);

    std::vector<std::vector<Game>> NFLSchedule;
    std::unordered_map<std::string, Team> teamMap;
};

#endif // NFLSIM_H
