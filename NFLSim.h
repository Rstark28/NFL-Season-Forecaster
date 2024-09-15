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
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "Game.h"

class NFLSim
{
public:
    NFLSim(const std::string &filename);
    ~NFLSim();

private:
    void runQueryLoop();
    void readSchedule(const std::string &filename);
    std::vector<std::string> processGameInfo(std::string teamName, std::string gameInfo, int week);
    void readTeams(const std::string &filename);
    void printSchedule() const;

    void simRegularSeason();

    void processAllGames();
    void processTeamGames(int teamIndex);
    void getHomeOddsStandard(std::shared_ptr<Game> &game);
    double calculateFieldAdvantage(const City &homeCity, const City &awayCity);
    double adjustEloForByes(const Game &game, const Team &homeTeam, const Team &awayTeam);
    double calculateHomeOdds(double eloDiff);

    void updateGame();
    void updateEloRatings(std::shared_ptr<Game> gamePtr);

    std::vector<std::vector<std::shared_ptr<Game>>> NFLSchedule;
    std::unordered_map<std::string, std::shared_ptr<Team>> teamMapByAbbreviation;
    std::unordered_map<int, std::shared_ptr<Team>> teamMapByIndex;
};

#endif // NFLSIM_H
