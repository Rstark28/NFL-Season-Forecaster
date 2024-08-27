#ifndef GAME_H
#define GAME_H

#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <tuple>
#include "Team.h"

class Game
{
public:
    Game(std::vector<std::string> tokens, const std::unordered_map<std::string, Team> &teamMap);
    ~Game();

    // Game comparator
    bool operator<(const Game &other) const;

    // Getter functions
    std::string printGame() const;
    Team getHomeTeam() const;
    Team getAwayTeam() const;
    bool getIsBye() const;
    bool getIsComplete() const;
    int getWeek() const;
    int getHomeTeamScore() const;
    int getAwayTeamScore() const;

    // Setter functions
    void setHomeTeamScore(int score);
    void setAwayTeamScore(int score);
    void setIsComplete(bool complete);

private:
    // Logic functions
    void getHomeOddsStandard();

    const Team &homeTeam;
    const Team &awayTeam;
    bool isBye = false;
    bool isComplete = false;
    int week;
    int homeTeamScore = 0;
    int awayTeamScore = 0;
};

#endif
