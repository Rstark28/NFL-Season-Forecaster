#include "Game.h"

Game::Game(std::vector<std::string> tokens, const std::unordered_map<std::string, std::shared_ptr<Team>> &teamMapByAbbreviation)
    : homeTeam(*teamMapByAbbreviation.at(tokens[1])),
      awayTeam(tokens[2] == "BYE" ? homeTeam : *teamMapByAbbreviation.at(tokens[2])),
      isBye(tokens[2] == "BYE"),
      isComplete(!isBye && tokens[3] == "Y"),
      week(std::stoi(tokens[0])),
      homeTeamScore(!isBye ? std::stoi(tokens[4]) : 0),
      awayTeamScore(!isBye ? std::stoi(tokens[5]) : 0)
{
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
           std::to_string(homeTeamScore) + "-" + std::to_string(awayTeamScore) + "|" + std::to_string(homeOdds);
}

const Team &Game::getHomeTeam() const
{
    return homeTeam;
}

const Team &Game::getAwayTeam() const
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

double Game::getHomeOdds() const
{
    return homeOdds;
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

void Game::setHomeOdds(double odds)
{
    homeOdds = odds;
}