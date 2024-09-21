#include "Team.h"

Team::Team()
    : name(""),
      abbreviation(""),
      color(""),
      eloRating(0.0),
      orgEloRating(0.0),
      city{"", 0.0, 0.0},
      scheduleIndex(0),
      winCount(0.0),
      playoffStatus(false),
      playoffRound(0)
{
}

Team::Team(std::string teamName, std::string abbreviation, std::string teamColor,
           double eloRating, std::string cityName, double lat, double lon, int scheduleIndex)
    : name(teamName),
      abbreviation(abbreviation),
      color(teamColor),
      eloRating(eloRating),
      orgEloRating(eloRating),
      city{cityName, lat, lon},
      scheduleIndex(scheduleIndex),
      winCount(0.0),
      playoffStatus(false),
      playoffRound(0)
{
}

Team::~Team() {}

// Getter function implementations

/**
 * @brief Get the name of the team.
 * @return The name of the team.
 */
std::string Team::getName() const
{
    return name;
}

/**
 * @brief Get the abbreviation of the team.
 * @return The abbreviation of the team.
 */
std::string Team::getAbbreviation() const
{
    return abbreviation;
}

/**
 * @brief Get the color of the team.
 * @return The color of the team.
 */
std::string Team::getColor() const
{
    return color;
}

/**
 * @brief Get the Elo rating of the team.
 * @return The Elo rating of the team.
 */
double Team::getEloRating() const
{
    return eloRating;
}

/**
 * @brief Get the city information of the team.
 * @return A reference to the City object containing city information.
 */
const City &Team::getCity() const
{
    return city;
}

/**
 * @brief Get the schedule index of the team.
 * @return The schedule index of the team.
 */
int Team::getScheduleIndex() const
{
    return scheduleIndex;
}

/**
 * @brief Get the win count of the team.
 * @return The win count of the team.
 */
float Team::getWinCount() const
{
    return winCount;
}

/**
 * @brief Check if the team has made the playoffs.
 * @return True if the team has made the playoffs, false otherwise.
 */
bool Team::hasMadePlayoffs() const
{
    return playoffStatus;
}

/**
 * @brief Get the playoff round the team is in.
 * @return The playoff round the team is in.
 */
int Team::getPlayoffRound() const
{
    return playoffRound;
}

// Setter function implementations

/**
 * @brief Update the Elo rating of the team.
 * @param eloChange The change to be applied to the Elo rating.
 */
void Team::updateEloRating(double eloChange)
{
    eloRating += eloChange;
}

/**
 * @brief Update the win count of the team.
 * @param result The result to be added to the win count.
 */
void Team::updateWinCount(float result)
{
    winCount += result;
}

/**
 * @brief Set the playoff status of the team.
 * @param madePlayoffs True if the team made the playoffs, false otherwise.
 */
void Team::setPlayoffStatus(bool madePlayoffs)
{
    playoffStatus = madePlayoffs;
}

/**
 * @brief Set the playoff round of the team.
 * @param round The playoff round to be set.
 */
void Team::setPlayoffRound(int round)
{
    playoffRound = round;
}

// Functions for tracking losses

/**
 * @brief Add a loss to the team's record.
 * @param opponent A shared pointer to the opponent team.
 * @param pointDifferential The point differential of the loss.
 */
void Team::addLoss(const std::shared_ptr<Team> &opponent, int pointDifferential)
{
    losses[opponent] = pointDifferential;
}

/**
 * @brief Get the losses of the team.
 * @return A reference to a map containing the losses of the team.
 */
const std::map<std::shared_ptr<Team>, int> &Team::getLosses() const
{
    return losses;
}

/**
 * @brief Reset the team's attributes to their original values.
 */
void Team::resetTeam()
{
    eloRating = orgEloRating;
    winCount = 0.0;
    playoffStatus = false;
    playoffRound = 0;
    losses.clear();
}