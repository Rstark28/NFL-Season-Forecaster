#include "Game.h"

Game::Game(std::vector<std::string> tokens, const std::unordered_map<std::string, std::shared_ptr<Team>> &teamMapByAbbreviation)
    : homeTeam(teamMapByAbbreviation.at(tokens[1])),
      awayTeam(tokens[2] == "BYE" ? homeTeam : teamMapByAbbreviation.at(tokens[2])),
      isBye(tokens[2] == "BYE"),
      isComplete(isBye || tokens[3] == "Y"),
      week(std::stoi(tokens[0])),
      homeTeamScore(!isBye ? std::stoi(tokens[4]) : 0),
      awayTeamScore(!isBye ? std::stoi(tokens[5]) : 0),
      homeOdds(0.0),
      fieldAdvantage(0.0),
      eloEffect(0.0)
{
}

Game::Game(const std::shared_ptr<Team> &homeTeam, const std::shared_ptr<Team> &awayTeam)
    : homeTeam(homeTeam),
      awayTeam(awayTeam),
      isBye(false),
      isComplete(false),
      homeTeamScore(0),
      awayTeamScore(0),
      homeOdds(0.0),
      fieldAdvantage(0.0),
      eloEffect(0.0)
{
}

Game::~Game()
{
}

std::string Game::printGame(const std::shared_ptr<Team> &primary) const
{
    if (isBye)
    {
        return "BYE";
    }
    if (homeTeam->getName() == primary->getName())
    {
        return awayTeam->getAbbreviation() + "|" + std::to_string(homeTeamScore) +
               "-" + std::to_string(awayTeamScore) + "|" + std::to_string(homeOdds * 100) + "%";
    }
    if (awayTeam->getName() == primary->getName())
    {
        return "@" + homeTeam->getAbbreviation() + "|" + std::to_string(awayTeamScore) +
               "-" + std::to_string(homeTeamScore) + "|" + std::to_string((1 - homeOdds) * 100) + "%";
    }
    return "Error: game not found";
}

std::shared_ptr<Team> Game::getHomeTeam() const
{
    return homeTeam;
}

std::shared_ptr<Team> Game::getAwayTeam() const
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

double Game::getFieldAdvantage() const
{
    return fieldAdvantage;
}

double Game::getEloEffect() const
{
    return eloEffect;
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

void Game::setFieldAdvantage(double advantage)
{
    fieldAdvantage = advantage;
}

void Game::setEloEffect(double eloChange)
{
    eloEffect = eloChange;
}
