#include "NFLSim.h"

/**
 * func::   NFLSim
 * param:   filename A string representing the name of the file containing the schedule information.
 * return:  None
 * info:    Constructor for the NFLSim class. Initializes the simulation by reading the NFL team data
 *          from a file and the game schedule from another file. Also processes all games to calculate
 *          initial odds.
 */
NFLSim::NFLSim(const std::string &filename)
{
    readTeams("nfl_teams.csv");
    readSchedule(filename);
    processAllGames();
}

/**
 * func::   ~NFLSim
 * param:   None
 * return:  None
 * info:    Destructor for the NFLSim class. Cleans up any resources used by the NFLSim class.
 */
NFLSim::~NFLSim() {}

/**
 * func::   readSchedule
 * param:   filename A string representing the name of the file containing the schedule information.
 * return:  None
 * info:    Reads the schedule from the specified file, ensuring that no duplicate games are added
 *          to the schedule. Each team's schedule is stored in the NFLSchedule member variable.
 */
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
            Game newGame(tokens, teamMapByAbbreviation);

            // Insert the newGame into the set and check if it was inserted
            auto result = uniqueGames.insert(newGame);
            if (result.second) // If the game was inserted
            {
                teamSchedule.push_back(newGame);
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

/**
 * func::   processGameInfo
 * param:   teamName A string representing the name of the team.
 * param:   gameInfo A string containing game information for a specific week.
 * param:   week An integer representing the week number of the game.
 * return:  A vector of strings containing processed game information, including week, team names, and game details.
 * info:    Processes the game information string by extracting relevant details, determining the home and away
 *          teams, and return:s a vector of strings with the processed game data.
 */
std::vector<std::string> NFLSim::processGameInfo(std::string teamName, std::string gameInfo, int week)
{
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

/**
 * func::   readTeams
 * param:   filename A string representing the name of the file containing the team data.
 * return:  None
 * info:    Reads the team data from the specified file, creates Team objects, and stores them in both teamMaps
 *          unordered_map using the team's abbreviation as the key.
 */
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

    int lineNumber = 0; // Initialize line number to track team index

    while (std::getline(file, line))
    {
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

        // Create a shared_ptr to a Team object
        auto team = std::make_shared<Team>(teamName, abbreviation, color, elo, city, latitude, longitude, lineNumber);

        // Add the team to both maps
        teamMapByAbbreviation[abbreviation] = team;
        teamMapByIndex[lineNumber] = team;

        ++lineNumber;
    }

    file.close();
}

/**
 * func::   printSchedule
 * param:   None
 * return:  None
 * info:    Prints the entire schedule in a formatted manner, ensuring each game is aligned for better readability.
 */
void NFLSim::printSchedule() const
{
    // Define column widths for formatting
    const int teamColumnWidth = 20;
    const int gameColumnWidth = 30;

    std::cout << std::left << std::setw(teamColumnWidth) << "Team" << " | " << "Games" << std::endl;
    std::cout << std::string(teamColumnWidth + gameColumnWidth + 3, '-') << std::endl;

    for (int i = 0; i < 32; ++i)
    {
        // Retrieve the team from the map using the index
        auto teamPair = teamMapByIndex.find(i);

        auto team = teamPair->second;

        // Print the team name
        std::cout << std::left << std::setw(teamColumnWidth) << team->getName() << " | ";

        // Retrieve and print the games for the current team from the schedule
        const auto &games = NFLSchedule[i];

        for (const auto &game : games)
        {
            std::cout << game.printGame() << ", ";
        }

        std::cout << std::endl;
    }
}

/**
 * func::   calculateDistance
 * param:   homeCity A reference to a City object representing the home team's city.
 * param:   awayCity A reference to a City object representing the away team's city.
 * return:  A double representing the distance in miles between the two cities.
 * info:    Calculates the distance between the home city and the away city using the Haversine formula
 *          and converts it from meters to miles.
 */
double NFLSim::calculateDistance(const City &homeCity, const City &awayCity)
{
    constexpr double R = 6378137.0; // Radius of the Earth in meters

    auto toRadians = [](double degrees)
    {
        return degrees * M_PI / 180.0;
    };

    double lat1 = toRadians(homeCity.latitude);
    double lon1 = toRadians(homeCity.longitude);
    double lat2 = toRadians(awayCity.latitude);
    double lon2 = toRadians(awayCity.longitude);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
                   std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    double distance = R * c;

    distance /= 1609.34; // Convert distance from meters to miles

    return distance; // Distance in miles
}

/**
 * func::   adjustEloForByes
 * param:   game A reference to a Game object representing the current game.
 * param:   homeTeam A reference to a Team object representing the home team.
 * param:   awayTeam A reference to a Team object representing the away team.
 * return:  A double representing the adjusted Elo difference accounting for bye weeks.
 * info:    Adjusts the Elo difference between the home and away teams, factoring in the effects
 *          of bye weeks on the team's performance.
 */
double NFLSim::adjustEloForByes(const Game &game, const Team &homeTeam, const Team &awayTeam)
{
    double elo_diff = homeTeam.getElo() - awayTeam.getElo();

    int homeTeamIdx = homeTeam.getSchedule();
    int awayTeamIdx = awayTeam.getSchedule();
    int week = game.getWeek();
    if (NFLSchedule[homeTeamIdx][week].getAwayTeam().getName() == homeTeam.getName())
    {
        elo_diff += 25;
    }
    if (NFLSchedule[awayTeamIdx][week].getAwayTeam().getName() == awayTeam.getName())
    {
        elo_diff -= 25;
    }

    return elo_diff;
}

/**
 * func::   calculateHomeOdds
 * param:   eloDiff A double representing the difference in two teams' elos.
 * return:  A double representing the homeOdds for a game object.
 * info:    Calculates homeOdds based on eloDiff var.
 */
double NFLSim::calculateHomeOdds(double eloDiff)
{
    return 1.0 / (1.0 + std::exp(-eloDiff / 400.0));
}

void NFLSim::getHomeOddsStandard(Game &game)
{
    const Team &homeTeam = game.getHomeTeam();
    const Team &awayTeam = game.getAwayTeam();

    // If bye week skip odds calculation
    if (homeTeam.getName() == awayTeam.getName())
    {
        return;
    }

    double eloDiff = adjustEloForByes(game, homeTeam, awayTeam);

    double distanceTraveled = calculateDistance(homeTeam.getCity(), awayTeam.getCity());
    eloDiff += distanceTraveled / 1000.0; // Convert meters to kilometers if needed

    double homeOdds = calculateHomeOdds(eloDiff);
    game.setHomeOdds(homeOdds);
}

/**
 * func::   processAllGames
 * param:   None
 * return:  None
 * info:    Calculates homeOdds for all unique game objects.
 */
void NFLSim::processAllGames()
{
    // Iterate over each week's schedule in NFLSchedule
    for (auto &weeklySchedule : NFLSchedule)
    {
        // Iterate over each game in the weekly schedule
        for (auto &game : weeklySchedule)
        {
            // Call getHomeOddsStandard for each game
            getHomeOddsStandard(game);
        }
    }
}
