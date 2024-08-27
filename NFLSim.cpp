#include "NFLSim.h"

NFLSim::NFLSim(const std::string &filename)
{
    readTeams("nfl_teams.csv");
    readSchedule(filename);
}

NFLSim::~NFLSim() {}

void NFLSim::readSchedule(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // Skip the first line (header)

    std::set<Game> uniqueGames; // Set to track unique games across all teams

    // Read each line of the file
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string teamName;
        std::getline(ss, teamName, ','); // Read team name

        std::vector<Game> teamSchedule; // Vector to hold games for the team
        std::string gameInfo;
        int week = 0; // Start week counter

        // Read each game information for the team
        while (std::getline(ss, gameInfo, ','))
        {
            std::vector<std::string> tokens = processGameInfo(teamName, gameInfo, week);

            // Create a new Game object
            Game newGame(tokens, teamMap);

            // Insert the newGame into the set and check if it was inserted
            auto result = uniqueGames.insert(newGame);
            if (result.second) // If the game was inserted
            {
                teamSchedule.push_back(newGame);
                getHomeOddsStandard(newGame);
            }
            else
            {
                teamSchedule.push_back(*result.first); // Use the existing game
            }
            ++week; // Move to the next week
        }
        NFLSchedule.push_back(teamSchedule);
    }

    file.close();
}

std::vector<std::string> NFLSim::processGameInfo(std::string teamName, std::string gameInfo, int week)
{
    // Split the gameInfo string by '#' symbols
    std::vector<std::string> tokens;
    std::stringstream ss(gameInfo);
    std::string token;

    tokens.push_back(std::to_string(week));
    tokens.push_back(teamName);
    while (std::getline(ss, token, '#'))
    {
        tokens.push_back(token);
    }

    // Determine the home and away teams based on the '@' symbol
    if (tokens.size() > 2 && tokens[2][0] == '@')
    {
        std::string tmp = tokens[1];
        tokens[1] = tokens[2].substr(1); // The team after '@' is the away team
        tokens[2] = tmp;                 // The current team is the home team
    }

    return tokens;
}

void NFLSim::readTeams(const std::string &filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return;
    }

    std::string line;
    std::getline(file, line); // Skip header

    int lineNumber = 0; // Initialize line number to track team idx

    while (std::getline(file, line))
    {
        ++lineNumber;

        std::stringstream ss(line);
        std::string teamName, abbreviation, color, city, eloStr, latStr, lonStr;

        std::getline(ss, teamName, ',');
        std::getline(ss, abbreviation, ',');
        std::getline(ss, color, ',');
        std::getline(ss, eloStr, ',');
        std::getline(ss, city, ',');
        std::getline(ss, latStr, ',');
        std::getline(ss, lonStr, ',');

        double elo = std::stod(eloStr);
        double latitude = std::stod(latStr);
        double longitude = std::stod(lonStr);

        // Create a Team object and add it to the unordered_map with team abbreviation as the key
        teamMap[abbreviation] = Team(teamName, abbreviation, color, elo, city, latitude, longitude, lineNumber);
    }

    file.close();
}

#include <iomanip> // For std::setw and std::left

void NFLSim::printSchedule() const
{
    // Determine the maximum width for the columns
    size_t maxWidth = 0;
    for (const auto &team : NFLSchedule)
    {
        for (const auto &game : team)
        {
            size_t gameLength = game.printGame().length();
            if (gameLength > maxWidth)
            {
                maxWidth = gameLength;
            }
        }
    }

    // Add some padding to the maximum width for better readability
    const size_t padding = 4; // Space between columns

    // Print the schedule with formatted output
    for (const auto &team : NFLSchedule)
    {
        for (const auto &game : team)
        {
            std::cout << std::left << std::setw(maxWidth + padding) << game.printGame();
        }
        std::cout << std::endl;
    }
}

double NFLSim::calculateDistance(const City &homeCity, const City &awayCity)
{
    // Radius of the Earth in meters
    constexpr double R = 6378137.0;

    // Convert degrees to radians
    auto toRadians = [](double degrees)
    {
        return degrees * M_PI / 180.0;
    };

    double lat1 = toRadians(homeCity.latitude);
    double lon1 = toRadians(homeCity.longitude);
    double lat2 = toRadians(awayCity.latitude);
    double lon2 = toRadians(awayCity.longitude);

    // Haversine formula
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
                   std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    double distance = R * c;

    // Convert distance from meters to miles
    distance /= 1609.34;

    return distance; // Distance in miles
}

double NFLSim::adjustEloForByes(const Game &game, const Team &homeTeam, const Team &awayTeam)
{
    double elo_diff = homeTeam.getElo() - awayTeam.getElo();

    // if (NFLSchedule[homeTeam.getSchedule()][game.getWeek()].getAwayTeam().getName() == "BYE")
    // {
    //     elo_diff += 25;
    // }
    // if (NFLSchedule[awayTeam.getSchedule()][game.getWeek()].getAwayTeam().getName() == "BYE")
    // {
    //     elo_diff -= 25;
    // }

    return elo_diff;
}

double NFLSim::calculateHomeOdds(double elo_diff)
{
    return 1.0 / (1.0 + std::exp(-elo_diff / 400.0));
}

void NFLSim::getHomeOddsStandard(const Game &game)
{
    const Team &homeTeam = game.getHomeTeam();
    const Team &awayTeam = game.getAwayTeam();

    if (homeTeam.getName() == awayTeam.getName())
    {
        return;
    }

    double eloDiff = adjustEloForByes(game, homeTeam, awayTeam);

    double distanceTraveled = calculateDistance(homeTeam.getCity(), awayTeam.getCity());
    eloDiff += distanceTraveled / 1000.0; // Convert meters to kilometers if needed

    double homeOdds = calculateHomeOdds(eloDiff);

    std::cout << homeOdds << " - " << homeTeam.getName() << "|" << awayTeam.getName() << std::endl;
}