#ifndef NFLSIM_H
#define NFLSIM_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
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
    void printLeagueStructure() const;
    void printPlayoffs() const;

    void simRegularSeason();
    void makePlayoffs();
    void getDivisionWinners();
    void getWildCard();
    std::shared_ptr<Team> resolveTiebreaker(const std::shared_ptr<Team> &team1,
                                            const std::shared_ptr<Team> &team2);
    void simulatePlayoffs();
    std::shared_ptr<Team> simPlayoffGame(std::shared_ptr<Team> homeTeam, std::shared_ptr<Team> awayTeam);

    void processAllGames();
    void processTeamGames(int teamIndex);
    void getHomeOddsStandard(std::shared_ptr<Game> &game);
    double calculateFieldAdvantage(const City &homeCity, const City &awayCity);
    double adjustEloForByes(const Game &game, const Team &homeTeam, const Team &awayTeam);
    double calculateHomeOdds(double eloDiff);

    void updateGame();
    void updateEloRatings(std::shared_ptr<Game> gamePtr);

    void simulateMultipleSeasons(int numSeasons);
    void printSeasonResults(const std::map<std::string, std::vector<int>> &teamWins, int season) const;
    void printFinalResults(const std::map<std::string, std::vector<int>> &teamWins, const std::map<std::string, std::vector<int>> &playoffRounds, int numSeasons) const;
    std::vector<std::vector<std::shared_ptr<Game>>> NFLSchedule;
    std::unordered_map<std::string, std::shared_ptr<Team>> teamMapByAbbreviation;
    std::unordered_map<int, std::shared_ptr<Team>> teamMapByIndex;
    std::map<std::string, std::map<std::string, std::vector<std::shared_ptr<Team>>>> leagueStructure;
    std::map<std::string, std::vector<std::shared_ptr<Team>>> playoffSeeding;
};

#endif // NFLSIM_H
