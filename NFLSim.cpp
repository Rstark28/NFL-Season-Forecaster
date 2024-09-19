
#include "NFLSim.h"

/**
 * @brief Constructor for the NFLSim class.
 *
 * This constructor initializes the NFL simulation by reading team data,
 * reading the schedule from a file, processing all games, and running the simulation.
 *
 * @param scheduleFilename The filename of the schedule CSV file.
 */
NFLSim::NFLSim(const std::string &scheduleFilename)
{
    // Read team data from a predefined CSV file
    readTeams("static/preseason_nfl_teams.csv");

    // Read the schedule from the provided filename
    readSchedule(scheduleFilename);

    // Process all games to calculate initial odds and Elo ratings
    processAllGames();

    // Run the simulation
    runSimulation();
}

NFLSim::~NFLSim() {}

/**
 * @brief Runs the main simulation loop, allowing user interaction.
 *
 * This function provides a command-line interface for the user to interact with the simulation.
 * The user can quit the simulation, update game results manually, print the schedule, or run multiple seasons.
 */
void NFLSim::runSimulation()
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
            manualGameResults();
        }
        else if (command == "print")
        {
            printSchedule();
        }
        else if (command == "run")
        {
            handleRunCommand();
        }
        else
        {
            std::cout << "Unknown command. Please try again." << std::endl;
        }
    }
}

/**
 * @brief Handles the "run" command to simulate multiple seasons.
 *
 * This function prompts the user to enter the number of seasons to simulate and then runs the simulation for that many seasons.
 */
void NFLSim::handleRunCommand()
{
    int numSeasons;
    std::cout << "Enter number of seasons to simulate: ";
    std::cin >> numSeasons;
    std::cin.ignore(); // Ignore newline character left in the input buffer
    simulateMultipleSeasons(numSeasons);
}

/**
 * @brief Reads the schedule from a CSV file and populates the NFLSchedule.
 *
 * This function reads the schedule from the provided CSV file, processes each line to extract game information,
 * and creates Game objects for each game. It updates the NFLSchedule with the parsed game data.
 *
 * @param filename The name of the CSV file containing the schedule.
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

    // Read each line of the file
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string teamName;
        std::getline(ss, teamName, ','); // Read team name

        std::vector<std::shared_ptr<Game>> teamSchedule; // Vector to hold game pointers for the team
        std::string gameInfo;
        int week = 0; // Start week counter

        // Read each game information for the team
        while (std::getline(ss, gameInfo, ','))
        {
            // Process the game information and create a new Game object wrapped in a shared_ptr
            std::vector<std::string> tokens = parseGameInfo(teamName, gameInfo, week);
            auto newGame = std::make_shared<Game>(tokens, teamMapByAbbreviation);

            // Check if the game object already exists, if it does push existing object again
            int minScheduleIdx = std::min(newGame->getHomeTeam()->getScheduleIndex(), newGame->getAwayTeam()->getScheduleIndex());
            if (minScheduleIdx < NFLSchedule.size())
            {
                teamSchedule.push_back(NFLSchedule[minScheduleIdx][week]);
            }
            else
            {
                teamSchedule.push_back(newGame);
                // Update Elos if game was completed in csv file
                if (newGame->isGameComplete())
                {
                    updateEloRatings(newGame);
                }
            }

            ++week; // Move to the next week
        }

        NFLSchedule.push_back(teamSchedule); // Add the team's schedule to NFLSchedule
    }

    file.close();
}

/**
 * @brief Parses game information for a specific team and week.
 *
 * This function takes a team's name, game information string, and the week number,
 * and parses the game information to extract relevant details such as the week number,
 * home team, and away team. It returns a vector of strings containing the parsed information.
 *
 * @param teamName The name of the team.
 * @param gameInfo The game information string.
 * @param week The week number.
 * @return A vector of strings containing the parsed game information.
 */
std::vector<std::string> NFLSim::parseGameInfo(const std::string &teamName, const std::string &gameInfo, int week)
{
    std::vector<std::string> parsedTokens;
    std::stringstream gameInfoStream(gameInfo);
    std::string token;

    // Add week number and team name to the parsed tokens
    parsedTokens.push_back(std::to_string(week));
    parsedTokens.push_back(teamName);

    // Split the game information string by '#' and add tokens to the parsed tokens
    while (std::getline(gameInfoStream, token, '#'))
    {
        parsedTokens.push_back(token);
    }

    // Determine the home and away teams based on the '@' symbol
    if (parsedTokens.size() > 2 && parsedTokens[2][0] == '@')
    {
        std::string homeTeam = parsedTokens[1];
        parsedTokens[1] = parsedTokens[2].substr(1); // The team after '@' is the away team
        parsedTokens[2] = homeTeam;                  // The current team is the home team
    }

    return parsedTokens;
}

/**
 * @brief Reads team data from a CSV file and initializes the team maps and league structure.
 *
 * This function reads team data from the provided CSV file, processes each line to extract team information,
 * and creates Team objects for each team. It updates the team maps and league structure with the parsed team data.
 *
 * @param filename The name of the CSV file containing the team data.
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

    int teamIndex = 0; // Initialize team index to track team position

    // Temporary maps to store the second version of team data
    std::map<std::string, std::shared_ptr<Team>> tempTeamMapByAbbreviation;
    std::map<int, std::shared_ptr<Team>> tempTeamMapByIndex;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string teamName, abbreviation, color, city, eloStr, latStr, lonStr, conference, division;

        // Read each field from the CSV file
        std::getline(ss, teamName, ',');
        std::getline(ss, abbreviation, ',');
        std::getline(ss, color, ',');
        std::getline(ss, eloStr, ',');
        std::getline(ss, city, ',');
        std::getline(ss, latStr, ',');
        std::getline(ss, lonStr, ',');
        std::getline(ss, conference, ','); // Ensure these fields are present in the file
        std::getline(ss, division, ',');   // Ensure these fields are present in the file

        double elo = std::stod(eloStr);
        double latitude = std::stod(latStr);
        double longitude = std::stod(lonStr);

        // Create a shared_ptr to a Team object
        auto team = std::make_shared<Team>(teamName, abbreviation, color, elo, city, latitude, longitude, teamIndex);

        // Add the team to both maps
        teamMapByAbbreviation[abbreviation] = team;
        teamMapByIndex[teamIndex] = team;

        // Add the team to the league structure by conference and division
        leagueStructure[conference][division].push_back(team);

        ++teamIndex;
    }

    file.close();

    // Optionally, you can now use tempTeamMapByAbbreviation and tempTeamMapByIndex for other purposes
}

/**
 * @brief Prints the schedule for all teams in the league.
 *
 * This function prints the schedule for each team in the league, including the team's name,
 * Elo rating, win count, and the details of each game in the schedule.
 */
void NFLSim::printSchedule() const
{
    // Define column widths for formatting
    const int teamColumnWidth = 20;
    const int weekColumnWidth = 7; // Width for "Week XX |"
    const int gameColumnWidth = 30;

    // Print header
    std::cout << std::left << std::setw(teamColumnWidth) << "Team" << " | " << "Games" << std::endl;
    std::cout << std::string(teamColumnWidth + weekColumnWidth + 3 + gameColumnWidth, '-') << std::endl;

    // Iterate over each team by index
    for (int teamIndex = 0; teamIndex < 32; ++teamIndex)
    {
        // Retrieve the team from the map using the index
        auto team = teamMapByIndex.at(teamIndex);

        // Print the team name, Elo rating, and win count
        printTeamHeader(team, teamColumnWidth, weekColumnWidth, gameColumnWidth);

        // Retrieve and print the games for the current team from the schedule
        const auto &games = NFLSchedule.at(teamIndex);
        printTeamGames(team, games, teamColumnWidth, weekColumnWidth, gameColumnWidth);
    }
}

/**
 * @brief Prints the header for a team, including the team's name, Elo rating, and win count.
 *
 * @param team The team object.
 * @param teamColumnWidth The width of the team column.
 * @param weekColumnWidth The width of the week column.
 * @param gameColumnWidth The width of the game column.
 */
void NFLSim::printTeamHeader(const std::shared_ptr<Team> &team, int teamColumnWidth, int weekColumnWidth, int gameColumnWidth) const
{
    std::cout << std::left << std::setw(teamColumnWidth) << team->getName() << " | Elo: " << team->getEloRating() << " | Wins: " << team->getWinCount() << std::endl;
    std::cout << std::string(teamColumnWidth + weekColumnWidth + 3 + gameColumnWidth, '-') << std::endl;
}

/**
 * @brief Prints the games for a team.
 *
 * @param team The team object.
 * @param games The vector of games for the team.
 * @param teamColumnWidth The width of the team column.
 * @param weekColumnWidth The width of the week column.
 * @param gameColumnWidth The width of the game column.
 */
void NFLSim::printTeamGames(const std::shared_ptr<Team> &team, const std::vector<std::shared_ptr<Game>> &games, int teamColumnWidth, int weekColumnWidth, int gameColumnWidth) const
{
    int weekIndex = 0;

    // Iterate over each game in the team's schedule
    for (const auto &game : games)
    {
        std::cout << std::left << std::setw(teamColumnWidth) << ("Week " + std::to_string(weekIndex))
                  << " | " << std::setw(gameColumnWidth) << game->getGameDetails(team) << std::endl;
        ++weekIndex;
    }

    std::cout << std::endl;
}

/**
 * @brief Calculates the field advantage based on the distance between two cities.
 *
 * This function calculates the field advantage for the home team based on the distance
 * between the home and away cities. The advantage is calculated using the Haversine formula
 * to determine the distance in miles, and then applying a point advantage based on the distance.
 *
 * @param homeCity The city of the home team.
 * @param awayCity The city of the away team.
 * @return The calculated field advantage in points.
 */
double NFLSim::calculateFieldAdvantage(const City &homeCity, const City &awayCity)
{
    constexpr double EARTH_RADIUS_METERS = 6378137.0; // Radius of the Earth in meters

    auto toRadians = [](double degrees)
    {
        return degrees * M_PI / 180.0;
    };

    double homeLatRad = toRadians(homeCity.latitude);
    double homeLonRad = toRadians(homeCity.longitude);
    double awayLatRad = toRadians(awayCity.latitude);
    double awayLonRad = toRadians(awayCity.longitude);

    double deltaLat = awayLatRad - homeLatRad;
    double deltaLon = awayLonRad - homeLonRad;
    double a = std::sin(deltaLat / 2) * std::sin(deltaLat / 2) +
               std::cos(homeLatRad) * std::cos(awayLatRad) *
                   std::sin(deltaLon / 2) * std::sin(deltaLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    double distanceMeters = EARTH_RADIUS_METERS * c;

    double distanceMiles = distanceMeters / 1609.34; // Convert distance from meters to miles

    double fieldAdvantage = distanceMiles / 1000 * 4 + 48; // 4 point advantage for every 1,000 miles, 48 for home field

    return fieldAdvantage;
}

/**
 * @brief Adjusts the Elo rating difference for bye weeks.
 *
 * This function adjusts the Elo rating difference between the home and away teams
 * based on whether either team has a bye week.
 *
 * @param game The game object.
 * @param homeTeam The home team.
 * @param awayTeam The away team.
 * @return The adjusted Elo rating difference.
 */
double NFLSim::adjustEloForByes(const Game &game, const Team &homeTeam, const Team &awayTeam)
{
    double eloDifference = homeTeam.getEloRating() - awayTeam.getEloRating();

    int homeTeamIndex = homeTeam.getScheduleIndex();
    int awayTeamIndex = awayTeam.getScheduleIndex();
    int week = game.getWeekNumber();

    if (NFLSchedule[homeTeamIndex][week]->getAwayTeam()->getName() == homeTeam.getName())
    {
        eloDifference += 25;
    }
    if (NFLSchedule[awayTeamIndex][week]->getAwayTeam()->getName() == awayTeam.getName())
    {
        eloDifference -= 25;
    }

    return eloDifference;
}

/**
 * @brief Calculates the probability of the home team winning based on Elo difference.
 *
 * This function calculates the probability of the home team winning a game based on
 * the difference in Elo ratings between the home and away teams.
 *
 * @param eloDifference The difference in Elo ratings between the home and away teams.
 * @return The probability of the home team winning.
 */
double NFLSim::calculateHomeOddsFromEloDiff(double eloDifference)
{
    return 1.0 / (1.0 + std::exp(-eloDifference / 400.0));
}

/**
 * @brief Calculates the home team odds for a game.
 *
 * This function calculates the odds of the home team winning a game by adjusting the
 * Elo rating difference for bye weeks and field advantage, and then calculating the
 * probability based on the adjusted Elo difference.
 *
 * @param game A shared pointer to the game object.
 */
void NFLSim::calculateHomeOdds(std::shared_ptr<Game> &game)
{
    // Access the home and away teams via shared pointers
    auto homeTeam = game->getHomeTeam();
    auto awayTeam = game->getAwayTeam();

    // If bye week, skip odds calculation
    if (homeTeam->getName() == awayTeam->getName())
    {
        return;
    }

    double eloDifference = adjustEloForByes(*game, *homeTeam, *awayTeam);

    // If distance hasn't been calculated before, do it; else grab the value
    if (game->getFieldAdvantage() == -1)
    {
        double fieldAdvantage = calculateFieldAdvantage(homeTeam->getCity(), awayTeam->getCity());
        game->setFieldAdvantage(fieldAdvantage);
        eloDifference += fieldAdvantage;
    }
    else
    {
        eloDifference += game->getFieldAdvantage();
    }

    double homeOdds = calculateHomeOddsFromEloDiff(eloDifference);
    game->setHomeTeamOdds(homeOdds);
}

/**
 * @brief Processes all games in the schedule to calculate initial odds.
 *
 * This function iterates over each week's schedule and each game within the week.
 * For each game that is not complete, it calculates the home team odds.
 */
void NFLSim::processAllGames()
{
    for (auto &weeklySchedule : NFLSchedule)
    {
        for (auto &gamePtr : weeklySchedule)
        {
            if (!gamePtr->isGameComplete())
            {
                calculateHomeOdds(gamePtr);
            }
        }
    }
}

/**
 * @brief Processes games for a specific team to calculate initial odds.
 *
 * This function retrieves the weekly schedule for the specified team and iterates over each game.
 * For each game that is not complete, it calculates the home team odds.
 *
 * @param teamIndex The index of the team in the schedule.
 */
void NFLSim::processTeamGames(int teamIndex)
{
    auto &weeklySchedule = NFLSchedule[teamIndex];

    for (auto &gamePtr : weeklySchedule)
    {
        if (!gamePtr->isGameComplete())
        {
            calculateHomeOdds(gamePtr);
        }
    }
}

/**
 * @brief Allows manual entry of game results and updates the simulation accordingly.
 *
 * This function prompts the user to enter a team abbreviation, game week, and score.
 * It then updates the game result, recalculates Elo ratings, and processes the games for both teams.
 */
void NFLSim::manualGameResults()
{
    std::string teamAbbreviation;
    int week;
    std::string score;

    std::cout << "Enter team abbreviation: ";
    std::getline(std::cin, teamAbbreviation);

    std::cout << "Enter game week (0-based index): ";
    std::cin >> week;
    std::cin.ignore(); // Ignore newline character left in the input buffer

    std::cout << "Enter score (format: homeScore-awayScore): ";
    std::getline(std::cin, score);

    size_t dashPos = score.find('-');
    if (dashPos == std::string::npos)
    {
        std::cerr << "Invalid score format. Use 'homeScore-awayScore'." << std::endl;
        return;
    }

    int homeScore = std::stoi(score.substr(0, dashPos));
    int awayScore = std::stoi(score.substr(dashPos + 1));

    auto teamIt = teamMapByAbbreviation.find(teamAbbreviation);
    if (teamIt == teamMapByAbbreviation.end())
    {
        std::cerr << "Team abbreviation not found." << std::endl;
        return;
    }

    auto team = teamIt->second;
    int scheduleIndex = team->getScheduleIndex();

    if (scheduleIndex < 0 || scheduleIndex > 31 || week < 0 || week > 18)
    {
        std::cerr << "Invalid week or schedule index." << std::endl;
        return;
    }

    auto &gamePtr = NFLSchedule[scheduleIndex][week];
    auto &game = *gamePtr;

    // If game has previosly been completed, reset the elo rating effects from
    // the previous update
    if (game.getEloRatingChange() != 0)
    {
        double eloChange = game.getEloRatingChange();
        game.getHomeTeam()->updateEloRating(-eloChange);
        game.getAwayTeam()->updateEloRating(eloChange);
    }

    // Reset game if score is 0-0
    if (homeScore == 0 and awayScore == 0)
    {
        game.setHomeTeamScore(0);
        game.setAwayTeamScore(0);
        game.setGameComplete(false);
        game.setEloRatingChange(0);
        processTeamGames(game.getHomeTeam()->getScheduleIndex());
        processTeamGames(game.getAwayTeam()->getScheduleIndex());
        std::cout << "Game reset." << std::endl;
        return;
    }

    game.setHomeTeamScore(homeScore);
    game.setAwayTeamScore(awayScore);
    game.setGameComplete(true);

    if (homeScore == awayScore)
    {
        game.getHomeTeam()->updateWinCount(0.5);
        game.getAwayTeam()->updateWinCount(0.5);
    }
    else
    {
        updateEloRatings(gamePtr);

        if (homeScore > awayScore)
        {
            game.getHomeTeam()->updateWinCount(1);
            game.getAwayTeam()->addLoss(game.getHomeTeam(), homeScore - awayScore);
        }
        else
        {
            game.getAwayTeam()->updateWinCount(1);
            game.getHomeTeam()->addLoss(game.getAwayTeam(), awayScore - homeScore);
        }
    }

    processTeamGames(game.getHomeTeam()->getScheduleIndex());
    processTeamGames(game.getAwayTeam()->getScheduleIndex());

    std::cout << "Game and Elo updated." << std::endl;
}

/**
 * @brief Updates the Elo ratings for a game based on the result.
 *
 * This function calculates the Elo rating adjustments for both teams based on the game result,
 * margin of victory, and other factors. It then updates the Elo ratings for both teams.
 *
 * @param gamePtr A shared pointer to the game object.
 */
void NFLSim::updateEloRatings(std::shared_ptr<Game> gamePtr)
{
    const double K = 4.0;                   // K-factor
    const double MOV_MULTIPLIER_BASE = 2.2; // Base for margin-of-victory multiplier
    const double MOV_SCALE = 0.001;         // Scaling factor for Elo difference

    auto &game = *gamePtr;

    auto homeTeamPtr = game.getHomeTeam();
    auto awayTeamPtr = game.getAwayTeam();

    if (!homeTeamPtr || !awayTeamPtr)
    {
        std::cerr << "Error: Null pointer for home or away team." << std::endl;
        return;
    }

    auto &homeTeam = *homeTeamPtr;
    auto &awayTeam = *awayTeamPtr;

    double homeElo = homeTeam.getEloRating();
    double awayElo = awayTeam.getEloRating();

    int homeScore = game.getHomeTeamScore();
    int awayScore = game.getAwayTeamScore();

    double eloDifference = homeElo - awayElo;
    double homeWinProbability = 1.0 / (1.0 + std::exp(-eloDifference / 400.0));

    double actualResult = (homeScore > awayScore) ? 1.0 : (homeScore < awayScore) ? 0.0
                                                                                  : 0.5;

    double forecastDelta = actualResult - homeWinProbability;

    double pointDifference = std::abs(homeScore - awayScore);
    double movMultiplier = std::log(pointDifference + 1) * MOV_MULTIPLIER_BASE;

    double eloAdjustment = movMultiplier * (eloDifference * MOV_SCALE + MOV_MULTIPLIER_BASE);

    double homeEloAdjustment = K * forecastDelta * eloAdjustment;
    double awayEloAdjustment = -homeEloAdjustment;

    homeElo += homeEloAdjustment;
    awayElo += awayEloAdjustment;

    homeTeam.updateEloRating(homeEloAdjustment);
    awayTeam.updateEloRating(awayEloAdjustment);

    game.setEloRatingChange(homeEloAdjustment);
}

/**
 * @brief Simulates the regular season games.
 *
 * This function iterates through each week and each game in the schedule,
 * generating random scores and determining the outcome of each game.
 * It updates the game results, Elo ratings, and processes the games for each team.
 * Finally, it determines the playoff teams and simulates the playoffs.
 */
void NFLSim::simulateRegularSeason()
{
    // Initialize the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Iterate through each week
    for (const auto &weeklyGames : NFLSchedule)
    {
        // Iterate through each game in the week
        for (const auto &game : weeklyGames)
        {
            // Skip if the game is already complete
            if (game->isGameComplete())
                continue;

            // Generate a random float between 0 and 1
            float randomValue = static_cast<float>(std::rand()) / RAND_MAX;

            // Generate scores using a log-linear distribution
            int homeScore = static_cast<int>(3 + 30 * std::log(1.0f + std::rand() / (static_cast<float>(RAND_MAX) + 1.0f)));
            int awayScore = static_cast<int>(3 + 30 * std::log(1.0f + std::rand() / (static_cast<float>(RAND_MAX) + 1.0f)));

            int winningScore, losingScore;
            std::shared_ptr<Team> winningTeam, losingTeam;

            // Determine if the game ends in a tie
            if (randomValue < 0.01f)
            {
                game->setHomeTeamScore(homeScore);
                game->setAwayTeamScore(homeScore); // Both teams get the same score
                game->getHomeTeam()->updateWinCount(0.5);
                game->getAwayTeam()->updateWinCount(0.5);
            }
            else
            {
                // Ensure that one score is higher than the other
                winningScore = std::max(homeScore, awayScore);
                losingScore = std::min(homeScore, awayScore);
                if (winningScore == losingScore)
                    winningScore++; // Avoid ties unless specified

                // Determine the winning and losing team
                if (randomValue > game->getHomeTeamOdds())
                {
                    game->setAwayTeamScore(winningScore);
                    game->setHomeTeamScore(losingScore);
                    winningTeam = game->getAwayTeam();
                    losingTeam = game->getHomeTeam();
                    winningTeam->updateWinCount(1);
                }
                else
                {
                    game->setHomeTeamScore(winningScore);
                    game->setAwayTeamScore(losingScore);
                    winningTeam = game->getHomeTeam();
                    losingTeam = game->getAwayTeam();
                    winningTeam->updateWinCount(1);
                }
            }

            // Mark the game as complete and update Elo ratings
            game->setGameComplete(true);
            updateEloRatings(game);

            // Process games for each team
            processTeamGames(game->getHomeTeam()->getScheduleIndex());
            processTeamGames(game->getAwayTeam()->getScheduleIndex());

            // Record the loss for the losing team
            if (winningTeam && losingTeam)
            {
                int pointDifferential = winningScore - losingScore;
                losingTeam->addLoss(winningTeam, pointDifferential);
            }
        }
    }

    // Determine division winners and print league structure
    determinePlayoffTeams();
    simulatePlayoffs();
}

/**
 * @brief Determines the playoff teams.
 *
 * This function determines the division winners and wildcard teams,
 * and sets the playoff status for each team.
 */
void NFLSim::determinePlayoffTeams()
{
    determineDivisionWinners();
    determineWildCardTeams();
    for (auto &team : teamMapByAbbreviation)
    {
        team.second->setPlayoffStatus(true);
    }
}

/**
 * @brief Determines the division winners for each conference.
 *
 * This function iterates through each conference and division,
 * sorts the teams by win count, and resolves any ties to determine
 * the top teams in each division. The division winners are added
 * to the playoff seeding.
 */
void NFLSim::determineDivisionWinners()
{
    // Data structure to store the top teams from each division for each conference
    std::map<std::string, std::vector<std::shared_ptr<Team>>> conferenceTeams;
    playoffSeeding.clear(); // Clear previous playoff seeding

    // Iterate through each conference
    for (const auto &conferencePair : leagueStructure)
    {
        const std::string &conference = conferencePair.first;

        // Vector to store top teams of this conference
        std::vector<std::shared_ptr<Team>> topTeams;

        // Iterate through each division in the conference
        for (const auto &divisionPair : conferencePair.second)
        {
            const std::vector<std::shared_ptr<Team>> &teams = divisionPair.second;

            // Create a copy of the teams vector and sort it by win count
            std::vector<std::shared_ptr<Team>> sortedTeams = teams;
            std::sort(sortedTeams.begin(), sortedTeams.end(),
                      [](const std::shared_ptr<Team> &a, const std::shared_ptr<Team> &b)
                      {
                          return a->getWinCount() > b->getWinCount();
                      });

            // Handle ties within the division
            if (sortedTeams.size() > 1)
            {
                std::vector<std::shared_ptr<Team>> topTeamsInDivision;

                // Compare top teams to resolve any ties
                for (size_t i = 0; i < sortedTeams.size(); ++i)
                {
                    for (size_t j = i + 1; j < sortedTeams.size(); ++j)
                    {
                        if (sortedTeams[i]->getWinCount() == sortedTeams[j]->getWinCount())
                        {
                            // Resolve tiebreaker between these two teams
                            auto winner = resolveTiebreaker(sortedTeams[i], sortedTeams[j]);
                            if (winner == sortedTeams[i])
                            {
                                topTeamsInDivision.push_back(sortedTeams[i]);
                            }
                            else
                            {
                                topTeamsInDivision.push_back(sortedTeams[j]);
                            }
                        }
                        else
                        {
                            topTeamsInDivision.push_back(sortedTeams[i]);
                            break; // Break once a non-tied team is added
                        }
                    }
                }

                // Ensure the top team of this division is unique
                if (!topTeamsInDivision.empty())
                {
                    topTeams.push_back(topTeamsInDivision.front());
                }
            }
            else if (!sortedTeams.empty())
            {
                // If no ties, just add the top team directly
                topTeams.push_back(sortedTeams.front());
            }
        }

        // Sort the top teams by win count
        std::sort(topTeams.begin(), topTeams.end(),
                  [](const std::shared_ptr<Team> &a, const std::shared_ptr<Team> &b)
                  {
                      return a->getWinCount() > b->getWinCount();
                  });

        // Store the sorted top teams for this conference
        conferenceTeams[conference] = topTeams;

        // Add division winners to playoff seeding
        if (conferenceTeams[conference].size() > 0)
        {
            playoffSeeding[conference] = conferenceTeams[conference]; // Add to playoff seeding
        }
    }
}

/**
 * @brief Determines the wildcard teams for each conference.
 *
 * This function iterates through each conference, identifies the non-division winners,
 * sorts them by win count, and selects the top teams for the wildcard spots.
 * The wildcard teams are then added to the playoff seeding.
 */
void NFLSim::determineWildCardTeams()
{
    // Data structure to store wildcard teams for each conference
    std::map<std::string, std::vector<std::shared_ptr<Team>>> wildcardTeams;

    // Clear the existing playoff seeding to prepare for wildcards
    for (auto &conferencePair : playoffSeeding)
    {
        const std::string &conference = conferencePair.first;
        auto &divisionWinners = conferencePair.second;

        // Get all teams excluding division winners
        std::vector<std::shared_ptr<Team>> nonPlayoffTeams;
        for (const auto &divisionPair : leagueStructure[conference])
        {
            for (const auto &team : divisionPair.second)
            {
                if (std::find(divisionWinners.begin(), divisionWinners.end(), team) == divisionWinners.end())
                {
                    nonPlayoffTeams.push_back(team);
                }
            }
        }

        // Sort non-playoff teams by win count
        std::sort(nonPlayoffTeams.begin(), nonPlayoffTeams.end(),
                  [](const std::shared_ptr<Team> &a, const std::shared_ptr<Team> &b)
                  {
                      return a->getWinCount() > b->getWinCount();
                  });

        // Select top 3 teams for wildcard spots
        std::vector<std::shared_ptr<Team>> topWildCardTeams;
        for (size_t i = 0; i < 3 && i < nonPlayoffTeams.size(); ++i)
        {
            topWildCardTeams.push_back(nonPlayoffTeams[i]);
        }

        // Store wildcard teams for this conference
        wildcardTeams[conference] = topWildCardTeams;

        // Add wildcard teams to playoff seeding
        playoffSeeding[conference].insert(playoffSeeding[conference].end(),
                                          topWildCardTeams.begin(), topWildCardTeams.end());
    }
}

/**
 * @brief Resolves a tiebreaker between two teams.
 *
 * This function determines the winner between two teams based on their losses and point differentials.
 * If the teams have not played each other or the point differentials are the same, a random choice is made.
 *
 * @param team1 The first team.
 * @param team2 The second team.
 * @return The team that wins the tiebreaker.
 */
std::shared_ptr<Team> NFLSim::resolveTiebreaker(const std::shared_ptr<Team> &team1,
                                                const std::shared_ptr<Team> &team2)
{
    auto team1Losses = team1->getLosses();
    auto team2Losses = team2->getLosses();

    // Seed the random number generator if not already seeded
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    // Check if both teams have played each other
    auto team1LostToTeam2 = team1Losses.find(team2);
    auto team2LostToTeam1 = team2Losses.find(team1);

    if (team1LostToTeam2 != team1Losses.end() && team2LostToTeam1 != team2Losses.end())
    {
        int team1PointDifferential = team1LostToTeam2->second;
        int team2PointDifferential = team2LostToTeam1->second;

        // The team that lost by less (i.e., had a smaller point differential) wins
        if (team1PointDifferential < team2PointDifferential)
        {
            return team1;
        }
        else if (team2PointDifferential < team1PointDifferential)
        {
            return team2;
        }
    }

    // If the teams have not played each other or point differentials are the same, return a random choice
    return (std::rand() % 2 == 0) ? team1 : team2;
}

/**
 * @brief Simulates the playoffs.
 *
 * This function simulates the playoff games for each conference and determines the conference champions.
 * It then simulates the Super Bowl between the AFC and NFC champions.
 */
void NFLSim::simulatePlayoffs()
{
    std::shared_ptr<Team> afcChampion, nfcChampion;

    // Iterate through each conference
    for (const auto &conferencePair : playoffSeeding)
    {
        const std::string &conference = conferencePair.first;
        std::vector<std::shared_ptr<Team>> teams = conferencePair.second;

        // Set initial playoff round for each team
        for (auto &team : teams)
        {
            team->setPlayoffRound(1);
        }

        // First round: 2nd seed vs 7th seed, 3rd seed vs 6th seed, 4th seed vs 5th seed
        std::vector<std::shared_ptr<Team>> round2;
        round2.push_back(teams[0]); // Top seed gets a bye
        round2.push_back(simulatePlayoffGame(teams[1], teams[6]));
        round2.push_back(simulatePlayoffGame(teams[2], teams[5]));
        round2.push_back(simulatePlayoffGame(teams[3], teams[4]));

        // Update teams' furthest playoff round
        for (auto &team : round2)
        {
            team->setPlayoffRound(2);
        }

        // Second round: Top seed vs lowest remaining seed, other two teams play each other
        std::sort(round2.begin() + 1, round2.end(),
                  [](const std::shared_ptr<Team> &a, const std::shared_ptr<Team> &b)
                  {
                      return a->getWinCount() < b->getWinCount();
                  });
        std::vector<std::shared_ptr<Team>> round3;
        round3.push_back(simulatePlayoffGame(round2[0], round2[1]));
        round3.push_back(simulatePlayoffGame(round2[2], round2[3]));

        // Update teams' furthest playoff round
        for (auto &team : round3)
        {
            team->setPlayoffRound(3);
        }

        // Conference championship
        std::shared_ptr<Team> conferenceChampion = simulatePlayoffGame(round3[0], round3[1]);
        std::cout << "Conference Champion (" << conference << "): " << conferenceChampion->getName() << std::endl;

        // Store the conference champion for the Super Bowl
        if (conference == "AFC")
        {
            afcChampion = conferenceChampion;
        }
        else if (conference == "NFC")
        {
            nfcChampion = conferenceChampion;
        }

        // Update the furthest playoff round for the conference champion
        conferenceChampion->setPlayoffRound(4);
    }

    // Super Bowl
    if (afcChampion && nfcChampion)
    {
        std::shared_ptr<Team> superBowlChampion = simulatePlayoffGame(afcChampion, nfcChampion);
        std::cout << "Super Bowl Champion: " << superBowlChampion->getName() << std::endl;

        // Update the furthest playoff round for the Super Bowl champion
        superBowlChampion->setPlayoffRound(5);
    }
}

/**
 * @brief Simulates a playoff game between two teams.
 *
 * This function creates a new game object for the playoff game, calculates the home team odds,
 * generates random scores, determines the winner, and updates the Elo ratings.
 *
 * @param homeTeam The home team.
 * @param awayTeam The away team.
 * @return The team that wins the playoff game.
 */
std::shared_ptr<Team> NFLSim::simulatePlayoffGame(std::shared_ptr<Team> homeTeam, std::shared_ptr<Team> awayTeam)
{
    // Create a new game object for the playoff game
    auto game = std::make_shared<Game>(homeTeam, awayTeam);

    // Calculate home odds based on Elo ratings and other factors
    calculateHomeOdds(game);

    // Generate a random float between 0 and 1
    float randomValue = static_cast<float>(std::rand()) / RAND_MAX;

    // Generate scores using a log-linear distribution
    int score1 = static_cast<int>(3 + 30 * std::log(1.0f + std::rand() / (static_cast<float>(RAND_MAX) + 1.0f)));
    int score2 = static_cast<int>(3 + 30 * std::log(1.0f + std::rand() / (static_cast<float>(RAND_MAX) + 1.0f)));

    int winningScore, losingScore;
    std::shared_ptr<Team> winningTeam, losingTeam;

    // Ensure that one score is higher than the other
    winningScore = std::max(score1, score2);
    losingScore = std::min(score1, score2);
    if (winningScore == losingScore)
        winningScore++; // Avoid ties unless specified

    // Determine the winning and losing team
    if (randomValue > game->getHomeTeamOdds())
    {
        game->setAwayTeamScore(winningScore);
        game->setHomeTeamScore(losingScore);
        winningTeam = game->getAwayTeam();
        losingTeam = game->getHomeTeam();
    }
    else
    {
        game->setHomeTeamScore(winningScore);
        game->setAwayTeamScore(losingScore);
        winningTeam = game->getHomeTeam();
        losingTeam = game->getAwayTeam();
    }

    // Mark the game as complete and update Elo ratings
    game->setGameComplete(true);
    updateEloRatings(game);

    // Print the result of the game
    std::cout << awayTeam->getAbbreviation() << game->getGameDetails(awayTeam) << std::endl;

    return winningTeam;
}

/**
 * @brief Simulates multiple NFL seasons and records the results.
 *
 * This function simulates a specified number of NFL seasons, recording the number of wins
 * and playoff rounds reached by each team in each season. It then prints the results for
 * each season and the final results in a table format.
 *
 * @param numSeasons The number of seasons to simulate.
 */
void NFLSim::simulateMultipleSeasons(int numSeasons)
{
    // Data structures to keep track of wins and playoff rounds for each team across seasons
    std::map<std::string, std::vector<int>> teamWins;
    std::map<std::string, std::vector<int>> playoffRounds;

    // Initialize the win counts and playoff rounds for each team
    for (const auto &teamPair : teamMapByAbbreviation)
    {
        teamWins[teamPair.first] = std::vector<int>(numSeasons, 0);
        playoffRounds[teamPair.first] = std::vector<int>(numSeasons, 0);
    }

    // Simulate each season
    for (int season = 0; season < numSeasons; ++season)
    {
        std::cout << "Simulating Season " << season + 1 << "..." << std::endl;

        // Simulate the regular season
        simulateRegularSeason();

        // Record the number of wins and playoff rounds for each team
        for (const auto &teamPair : teamMapByAbbreviation)
        {
            teamWins[teamPair.first][season] = teamPair.second->getWinCount();
            playoffRounds[teamPair.first][season] = teamPair.second->getPlayoffRound();
        }
    }

    // Print the final results in a table format
    printFinalResults(teamWins, playoffRounds, numSeasons);
}

/**
 * @brief Prints the final results of all simulated seasons.
 *
 * This function prints the number of wins for each team in each season, calculates
 * the probabilities of reaching different playoff rounds, and prints the average
 * number of wins for each team across all simulated seasons.
 *
 * @param teamWins A map containing the number of wins for each team across all seasons.
 * @param playoffRounds A map containing the playoff rounds reached by each team across all seasons.
 * @param numSeasons The number of seasons simulated.
 */
void NFLSim::printFinalResults(const std::map<std::string, std::vector<int>> &teamWins, const std::map<std::string, std::vector<int>> &playoffRounds, int numSeasons) const
{
    // Calculate and print playoff probabilities
    std::cout << std::left << std::setw(15) << "Team" << " | " << "Avg Wins" << " | " << "WildCard" << " | " << "Divisional" << " | " << "Conference" << " | " << "Super Bowl" << " | " << "Championships" << std::endl;
    std::cout << std::string(95, '-') << std::endl;

    for (const auto &teamPair : teamWins)
    {
        const std::string &teamName = teamPair.first;
        const std::vector<int> &wins = teamPair.second;
        const std::vector<int> &rounds = playoffRounds.at(teamName);

        double totalWins = std::accumulate(wins.begin(), wins.end(), 0.0);
        double averageWins = totalWins / numSeasons;

        int wildCardCount = 0;
        int divisionalCount = 0;
        int conferenceCount = 0;
        int superBowlCount = 0;
        int championshipCount = 0;

        for (int round : rounds)
        {
            if (round >= 1)
                wildCardCount++;
            if (round >= 2)
                divisionalCount++;
            if (round >= 3)
                conferenceCount++;
            if (round >= 4)
                superBowlCount++;
            if (round == 5)
                championshipCount++;
        }

        double wildCardProb = static_cast<double>(wildCardCount) / numSeasons;
        double divisionalProb = static_cast<double>(divisionalCount) / numSeasons;
        double conferenceProb = static_cast<double>(conferenceCount) / numSeasons;
        double superBowlProb = static_cast<double>(superBowlCount) / numSeasons;
        double championshipProb = static_cast<double>(championshipCount) / numSeasons;

        std::cout << std::left << std::setw(15) << teamName
                  << " | " << std::setw(8) << averageWins
                  << " | " << std::setw(10) << wildCardProb
                  << " | " << std::setw(10) << divisionalProb
                  << " | " << std::setw(10) << conferenceProb
                  << " | " << std::setw(10) << superBowlProb
                  << " | " << std::setw(10) << championshipProb << std::endl;
    }
}
