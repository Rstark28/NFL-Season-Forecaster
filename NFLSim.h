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
#include <iomanip>
#include <memory>

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
    void getHomeOddsStandard(Game &game);
    void processAllGames();

private:
    double calculateDistance(const City &homeCity, const City &awayCity);
    double adjustEloForByes(const Game &game, const Team &homeTeam, const Team &awayTeam);
    double calculateHomeOdds(double eloDiff);

    std::vector<std::vector<Game>> NFLSchedule;
    std::unordered_map<std::string, std::shared_ptr<Team>> teamMapByAbbreviation;
    std::unordered_map<int, std::shared_ptr<Team>> teamMapByIndex;
};

#endif // NFLSIM_H
