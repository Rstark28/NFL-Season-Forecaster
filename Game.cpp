#include "Game.h"

Game::Game(std::vector<std::string> tokens, const std::unordered_map<std::string, Team> &teamMap)
{
    week = stoi(tokens[0]);
    homeTeam = teamMap.find(tokens[1])->second;
    if (tokens[2] == "BYE")
    {
        isBye = true;
        return;
    }
    awayTeam = teamMap.find(tokens[2])->second;
    isComplete = (tokens[3] == "Y");
    homeTeamScore = stoi(tokens[4]);
    awayTeamScore = stoi(tokens[5]);
}

Game::~Game()
{
}

bool Game::operator<(const Game &other) const
{
    std::string homeName = homeTeam.getName();
    std::string awayName = awayTeam.getName();
    std::string otherHomeName = other.homeTeam.getName();
    std::string otherAwayName = other.awayTeam.getName();

    return std::tie(week, homeName, awayName) <
           std::tie(other.week, otherHomeName, otherAwayName);
}

void Game::getHomeOddsStandard()
{
    // double eloDiff = homeTeam.elo - awayTeam.elo;
}
std::string Game::printGame() const
{
    if (isBye)
    {
        return "BYE";
    }

    return awayTeam.getAbbreviation() + "@" + homeTeam.getAbbreviation() +
           std::to_string(homeTeamScore) + "-" + std::to_string(awayTeamScore);
}

Team Game::getHomeTeam() const
{
    return homeTeam;
}

Team Game::getAwayTeam() const
{
    return awayTeam;
}

bool Game::getIsBye() const
{
    return isBye;
}

bool Game::getIsComplete() const
{
    return isComplete;
}

int Game::getWeek() const
{
    return week;
}

int Game::getHomeTeamScore() const
{
    return homeTeamScore;
}

int Game::getAwayTeamScore() const
{
    return awayTeamScore;
}

// Setter functions
void Game::setHomeTeamScore(int score)
{
    homeTeamScore = score;
}

void Game::setAwayTeamScore(int score)
{
    awayTeamScore = score;
}

void Game::setIsComplete(bool complete)
{
    isComplete = complete;
}