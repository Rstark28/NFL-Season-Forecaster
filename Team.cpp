#include "Team.h"

Team::Team()
    : name(""),
      abbrev(""),
      color(""),
      elo(0.0),
      city{"", 0.0, 0.0},
      scheduleIdx(0)
{
}

Team::Team(std::string teamName, std::string abbreviation, std::string teamColor,
           double eloRating, std::string cityName, double lat, double lon, int schedule)
    : name(teamName),
      abbrev(abbreviation),
      color(teamColor),
      elo(eloRating),
      city{cityName, lat, lon},
      scheduleIdx{schedule}
{
}

Team::~Team() {}

// Getter function implementations
std::string Team::getName() const
{
    return name;
}

std::string Team::getAbbreviation() const
{
    return abbrev;
}

std::string Team::getColor() const
{
    return color;
}

double Team::getElo() const
{
    return elo;
}

const City &Team::getCity() const
{
    return city;
}

int Team::getSchedule() const
{
    return scheduleIdx;
}