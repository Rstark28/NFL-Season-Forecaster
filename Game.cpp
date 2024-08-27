#include "Game.h"

Game::Game(std::vector<std::string> tokens, const std::unordered_map<std::string, Team> &teamMap)
    : homeTeam(teamMap.at(tokens[1])),
      awayTeam(tokens[2] == "BYE" ? teamMap.at(tokens[1]) : teamMap.at(tokens[2])),
      isBye(tokens[2] == "BYE"),
      isComplete(!isBye && tokens[3] == "Y"),
      week(stoi(tokens[0])),
      homeTeamScore(!isBye ? stoi(tokens[4]) : 0),
      awayTeamScore(!isBye ? stoi(tokens[5]) : 0)
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