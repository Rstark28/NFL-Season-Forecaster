#include "Team.h"

Team::Team()
{
}

Team::Team(std::string teamName, std::string abbreviation, std::string teamColor,
           double eloRating, std::string cityName, double lat, double lon)
{
    name = teamName;
    abbrev = abbreviation;
    color = teamColor;
    elo = eloRating;
    city.name = cityName;
    city.latitude = lat;
    city.longitude = lon;
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
