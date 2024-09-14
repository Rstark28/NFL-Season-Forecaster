#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Team.h"

class Game
{
public:
    Game(std::vector<std::string> tokens, const std::unordered_map<std::string, std::shared_ptr<Team>> &teamMapByAbbreviation);
    ~Game();

    // Game comparator
    bool operator<(const Game &other) const;

    // Getter functions
    std::string printGame(const Team &primary) const;
    Team &getHomeTeam() const;
    Team &getAwayTeam() const;

    bool getIsBye() const;
    bool getIsComplete() const;
    int getWeek() const;
    int getHomeTeamScore() const;
    int getAwayTeamScore() const;
    double getHomeOdds() const;
    double getFieldAdvantage() const;

    // Setter functions
    void setHomeTeamScore(int score);
    void setAwayTeamScore(int score);
    void setIsComplete(bool complete);
    void setHomeOdds(double odds);
    void setFieldAdvantage(double advantage);

private:
    // Logic functions
    void getHomeOddsStandard();

    Team &homeTeam;
    Team &awayTeam;
    bool isBye = false;
    bool isComplete = false;
    int week;
    int homeTeamScore = 0;
    int awayTeamScore = 0;
    double homeOdds;
    double fieldAdvantage = -1;
};

#endif // GAME_H
