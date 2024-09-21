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
    // Constructors
    Game(std::vector<std::string> tokens, const std::unordered_map<std::string, std::shared_ptr<Team>> &teamMapByAbbreviation);
    Game(const std::shared_ptr<Team> &homeTeam, const std::shared_ptr<Team> &awayTeam);
    ~Game();

    // Getter functions
    std::string getGameDetails(const std::shared_ptr<Team> &primaryTeam) const;
    std::string getCSVDetails(const std::shared_ptr<Team> &primaryTeam) const;
    std::shared_ptr<Team> getHomeTeam() const;
    std::shared_ptr<Team> getAwayTeam() const;

    bool isByeWeek() const;
    bool isGameComplete() const;
    int getWeekNumber() const;
    int getHomeTeamScore() const;
    int getAwayTeamScore() const;
    double getHomeTeamOdds() const;
    double getFieldAdvantage() const;
    double getEloRatingChange() const;
    bool isUserSet() const;

    // Setter functions
    void setHomeTeamScore(int score);
    void setAwayTeamScore(int score);
    void setGameComplete(bool complete);
    void setHomeTeamOdds(double odds);
    void setFieldAdvantage(double advantage);
    void setEloRatingChange(double eloChange);
    void setUserSet(bool isUserSet);
    void resetGame();

private:
    std::shared_ptr<Team> homeTeam; // Home team
    std::shared_ptr<Team> awayTeam; // Away team
    bool byeWeek = false;           // Indicates if it's a bye week
    bool gameComplete = false;      // Indicates if the game is complete
    int weekNumber;                 // Week number of the game
    int homeTeamScore = 0;          // Score of the home team
    int awayTeamScore = 0;          // Score of the away team
    double homeTeamOdds;            // Odds for the home team
    double fieldAdvantage = -1;     // Field advantage value
    double eloRatingChange = 0;     // Change in Elo rating
    bool userSet;                   // Indicates if the game result was set by the user
};

#endif // GAME_H
