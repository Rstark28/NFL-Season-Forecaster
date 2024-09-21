#include "Game.h"

// Constructor that initializes a Game object from a vector of tokens and a map of teams by abbreviation
/**
 * @brief Constructs a Game object.
 * @param tokens A vector of strings containing game details.
 * @param teamMapByAbbreviation A map of team abbreviations to Team objects.
 */
Game::Game(std::vector<std::string> tokens, const std::unordered_map<std::string, std::shared_ptr<Team>> &teamMapByAbbreviation)
    : homeTeam(teamMapByAbbreviation.at(tokens[1])),
      awayTeam(tokens[2] == "BYE" ? homeTeam : teamMapByAbbreviation.at(tokens[2])),
      byeWeek(tokens[2] == "BYE"),
      gameComplete(byeWeek || tokens[3] == "Y"),
      weekNumber(std::stoi(tokens[0])),
      homeTeamScore(!byeWeek ? std::stoi(tokens[4]) : 0),
      awayTeamScore(!byeWeek ? std::stoi(tokens[5]) : 0),
      homeTeamOdds(0.0),
      fieldAdvantage(0.0),
      eloRatingChange(0.0)
{
}

// Constructor that initializes a Game object with home and away teams
/**
 * @brief Constructs a Game object.
 * @param homeTeam A shared pointer to the home team.
 * @param awayTeam A shared pointer to the away team.
 */
Game::Game(const std::shared_ptr<Team> &homeTeam, const std::shared_ptr<Team> &awayTeam)
    : homeTeam(homeTeam),
      awayTeam(awayTeam),
      byeWeek(false),
      gameComplete(false),
      homeTeamScore(0),
      awayTeamScore(0),
      homeTeamOdds(0.0),
      fieldAdvantage(0.0),
      eloRatingChange(0.0)
{
}

// Destructor for the Game class
/**
 * @brief Destroys the Game object.
 */
Game::~Game()
{
}

// Function to print the game details
/**
 * @brief Prints the game details.
 * @param primary A shared pointer to the primary team.
 * @return A string representing the game details.
 */
std::string Game::getGameDetails(const std::shared_ptr<Team> &primary) const
{
    if (byeWeek)
    {
        return "BYE";
    }
    if (homeTeam->getName() == primary->getName())
    {
        return awayTeam->getAbbreviation() + "|" + std::to_string(homeTeamScore) +
               "-" + std::to_string(awayTeamScore) + "|" + std::to_string(homeTeamOdds * 100) + "%";
    }
    if (awayTeam->getName() == primary->getName())
    {
        return "@" + homeTeam->getAbbreviation() + "|" + std::to_string(awayTeamScore) +
               "-" + std::to_string(homeTeamScore) + "|" + std::to_string((1 - homeTeamOdds) * 100) + "%";
    }
    return "Error: game not found";
}

// Function to print the game details in CSV format
/**
 * @brief Prints the game details in CSV format.
 * @param primary A shared pointer to the primary team.
 * @return A string representing the game details in CSV format.
 */
std::string Game::getCSVDetails(const std::shared_ptr<Team> &primary) const
{
    std::string result;
    if (byeWeek)
    {
        result = "BYE#N#0#0";
    }
    else
    {
        std::string teamAbbreviation;
        if (homeTeam->getName() == primary->getName())
        {
            teamAbbreviation = awayTeam->getAbbreviation();
        }
        else if (awayTeam->getName() == primary->getName())
        {
            teamAbbreviation = "@" + homeTeam->getAbbreviation();
        }
        else
        {
            return "Error: game not found";
        }

        result = teamAbbreviation;
        result += "#";
        result += (gameComplete ? "Y" : "N");
        result += "#";
        result += std::to_string(homeTeamScore);
        result += "#";
        result += std::to_string(awayTeamScore);
    }

    return result;
}

// Getter for the home team
/**
 * @brief Gets the home team.
 * @return A shared pointer to the home team.
 */
std::shared_ptr<Team> Game::getHomeTeam() const
{
    return homeTeam;
}

// Getter for the away team
/**
 * @brief Gets the away team.
 * @return A shared pointer to the away team.
 */
std::shared_ptr<Team> Game::getAwayTeam() const
{
    return awayTeam;
}

// Getter for the byeWeek flag
/**
 * @brief Checks if the game is a bye week.
 * @return True if the game is a bye week, false otherwise.
 */
bool Game::isByeWeek() const
{
    return byeWeek;
}

// Getter for the gameComplete flag
/**
 * @brief Checks if the game is complete.
 * @return True if the game is complete, false otherwise.
 */
bool Game::isGameComplete() const
{
    return gameComplete;
}

// Getter for the week
/**
 * @brief Gets the week of the game.
 * @return The week of the game.
 */
int Game::getWeekNumber() const
{
    return weekNumber;
}

// Getter for the home team score
/**
 * @brief Gets the home team score.
 * @return The home team score.
 */
int Game::getHomeTeamScore() const
{
    return homeTeamScore;
}

// Getter for the away team score
/**
 * @brief Gets the away team score.
 * @return The away team score.
 */
int Game::getAwayTeamScore() const
{
    return awayTeamScore;
}

// Getter for the home odds
/**
 * @brief Gets the home team odds.
 * @return The home team odds.
 */
double Game::getHomeTeamOdds() const
{
    return homeTeamOdds;
}

// Getter for the field advantage
/**
 * @brief Gets the field advantage.
 * @return The field advantage.
 */
double Game::getFieldAdvantage() const
{
    return fieldAdvantage;
}

// Getter for the elo effect
/**
 * @brief Gets the elo effect.
 * @return The elo effect.
 */
double Game::getEloRatingChange() const
{
    return eloRatingChange;
}

// Getter for the userSet flag
/**
 * @brief Checks if the game result was set by the user.
 * @return True if the game result was set by the user, false otherwise.
 */
bool Game::isUserSet() const
{
    return userSet;
}

// Setter for the home team score
/**
 * @brief Sets the home team score.
 * @param score The home team score.
 */
void Game::setHomeTeamScore(int score)
{
    homeTeamScore = score;
}

// Setter for the away team score
/**
 * @brief Sets the away team score.
 * @param score The away team score.
 */
void Game::setAwayTeamScore(int score)
{
    awayTeamScore = score;
}

// Setter for the gameComplete flag
/**
 * @brief Sets the gameComplete flag.
 * @param complete The gameComplete flag.
 */
void Game::setGameComplete(bool complete)
{
    gameComplete = complete;
}

// Setter for the home odds
/**
 * @brief Sets the home team odds.
 * @param odds The home team odds.
 */
void Game::setHomeTeamOdds(double odds)
{
    homeTeamOdds = odds;
}

// Setter for the field advantage
/**
 * @brief Sets the field advantage.
 * @param advantage The field advantage.
 */
void Game::setFieldAdvantage(double advantage)
{
    fieldAdvantage = advantage;
}

// Setter for the elo effect
/**
 * @brief Sets the elo effect.
 * @param eloChange The elo effect.
 */
void Game::setEloRatingChange(double eloChange)
{
    eloRatingChange = eloChange;
}

// Setter for the userSet flag
/**
 * @brief Sets the userSet flag.
 * @param isUserSet The userSet flag.
 */
void Game::setUserSet(bool isUserSet)
{
    userSet = isUserSet;
}

// Function to reset the game
/**
 * @brief Resets the game.
 */
void Game::resetGame()
{
    homeTeamScore = 0;
    awayTeamScore = 0;
    gameComplete = byeWeek ? true : false;
    eloRatingChange = 0;
}