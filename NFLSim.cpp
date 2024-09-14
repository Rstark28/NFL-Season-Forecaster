#include "NFLSim.h"

/**
 * func::   NFLSim
 * param:   filename A string representing the name of the file containing the schedule information.
 * return:  None
 * info:    Constructor for the NFLSim class. Initializes the simulation by reading the NFL team data
 *          from a file and the game schedule from another file. Also processes all games to calculate
 *          initial odds. Starts the query loop to handle user commands.
 */
NFLSim::NFLSim(const std::string &filename)
{
    readTeams("nfl_teams.csv");
    readSchedule(filename);
    processAllGames();
    runQueryLoop();
}

/**
 * func::   ~NFLSim
 * param:   None
 * return:  None
 * info:    Destructor for the NFLSim class. Cleans up any resources used by the NFLSim class.
 */
NFLSim::~NFLSim() {}

/**
 * func::   runQueryLoop
 * param:   None
 * return:  None
 * info:    Runs a query loop that processes user commands to quit, update games, print the schedule,
 *          or run the simulation.
 */
void NFLSim::runQueryLoop()
{
    std::string command;

    while (true)
    {
        std::cout << "Enter command (quit, update, print, run): ";
        std::getline(std::cin, command);

        if (command == "quit")
        {
            break;
        }
        else if (command == "update")
        {
            updateGame();
        }
        else if (command == "print")
        {
            printSchedule();
        }
        else if (command == "run")
        {
            // Call method to run the simulation (not implemented)
            std::cout << "Run simulation functionality is not yet implemented." << std::endl;
        }
        else
        {
            std::cout << "Unknown command. Please try again." << std::endl;
        }
    }
}

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
    const int weekColumnWidth = 7; // Width for "Week XX |"
    const int gameColumnWidth = 30;

    std::cout << std::left << std::setw(teamColumnWidth) << "Team" << " | " << "Games" << std::endl;
    std::cout << std::string(teamColumnWidth + weekColumnWidth + 3 + gameColumnWidth, '-') << std::endl;

    for (int i = 0; i < 32; ++i)
    {
        // Retrieve the team from the map using the index
        auto team = teamMapByIndex.at(i);

        // Print the team name
        std::cout << std::left << std::setw(teamColumnWidth) << team->getName() << " | Elo: " << team->getElo() << std::endl;
        std::cout << std::string(teamColumnWidth + weekColumnWidth + 3 + gameColumnWidth, '-') << std::endl;

        // Retrieve and print the games for the current team from the schedule
        const auto &games = NFLSchedule.at(i);

        int weekIndex = 0;

        for (const auto &game : games)
        {
            std::cout << std::left << std::setw(teamColumnWidth) << ("Week " + std::to_string(weekIndex))
                      << " | " << std::setw(gameColumnWidth) << game.printGame(*team) << std::endl;
            ++weekIndex;
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
            if (!game.getIsComplete())
            {
                getHomeOddsStandard(game);
            }
        }
    }
}

/**
 * func::   updateGame
 * param:   None
 * return:  None
 * info:    Sets game outcome.
 */
void NFLSim::updateGame()
{
    std::string teamAbbrev;
    int week;
    std::string score;

    // Prompt for team abbreviation
    std::cout << "Enter team abbreviation: ";
    std::getline(std::cin, teamAbbrev);

    // Prompt for game week
    std::cout << "Enter game week (0-based index): ";
    std::cin >> week;
    std::cin.ignore(); // Ignore newline character left in the input buffer

    // Prompt for score
    std::cout << "Enter score (format: homeScore-awayScore): ";
    std::getline(std::cin, score);

    // Parse the score
    size_t dashPos = score.find('-');
    if (dashPos == std::string::npos)
    {
        std::cerr << "Invalid score format. Use 'homeScore-awayScore'." << std::endl;
        return;
    }

    int homeScore = std::stoi(score.substr(0, dashPos));
    int awayScore = std::stoi(score.substr(dashPos + 1));

    // Find the team using the abbreviation
    auto teamIt = teamMapByAbbreviation.find(teamAbbrev);
    if (teamIt == teamMapByAbbreviation.end())
    {
        std::cerr << "Team abbreviation not found." << std::endl;
        return;
    }

    auto team = teamIt->second;
    int scheduleIdx = team->getSchedule(); // Get the schedule index for the team

    // Check if the week is valid
    if (scheduleIdx < 0 || scheduleIdx > 31 || week < 0 || week > 18)
    {
        std::cerr << "Invalid week or schedule index." << std::endl;
        return;
    }

    // Retrieve the game and update the scores
    Game &game = NFLSchedule[scheduleIdx][week];
    game.setHomeTeamScore(homeScore);
    game.setAwayTeamScore(awayScore);
    game.setIsComplete(true);
    updateEloRatings(game);
    processAllGames();

    std::cout << "Game and elo updated." << std::endl;
}

void NFLSim::updateEloRatings(const Game &game)
{
    const double K = 20.0;                  // K-factor
    const double MOV_MULTIPLIER_BASE = 2.2; // Base for margin-of-victory multiplier
    const double MOV_SCALE = 0.001;         // Scaling factor for Elo difference

    // Get home and away teams
    auto &homeTeam = game.getHomeTeam();
    auto &awayTeam = game.getAwayTeam();

    double homeElo = homeTeam.getElo();
    double awayElo = awayTeam.getElo();

    int homeScore = game.getHomeTeamScore();
    int awayScore = game.getAwayTeamScore();

    // Calculate the expected probability of home team winning
    double eloDiff = homeElo - awayElo;
    double homeWinProbability = 1.0 / (1.0 + std::exp(-eloDiff / 400.0));

    // Determine the actual result
    double actualResult = (homeScore > awayScore) ? 1.0 : (homeScore < awayScore) ? 0.0
                                                                                  : 0.5;

    // Calculate forecast delta
    double forecastDelta = actualResult - homeWinProbability;

    // Calculate margin of victory multiplier
    double pointDiff = std::abs(homeScore - awayScore);
    double movMultiplier = std::log(pointDiff + 1) * MOV_MULTIPLIER_BASE;

    // Calculate the Elo adjustment
    double eloAdjustment = movMultiplier * (eloDiff * MOV_SCALE + MOV_MULTIPLIER_BASE);

    // Adjust Elo ratings
    double homeEloAdjustment = K * forecastDelta * eloAdjustment;
    double awayEloAdjustment = -homeEloAdjustment;

    homeElo += homeEloAdjustment;
    awayElo += awayEloAdjustment;

    // Update team ratings
    homeTeam.setElo(homeElo);
    awayTeam.setElo(awayElo);
}
