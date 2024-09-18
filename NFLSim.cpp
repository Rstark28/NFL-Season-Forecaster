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
    readTeams("preseason_nfl_teams.csv");
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
            simRegularSeason();
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
            std::vector<std::string> tokens = processGameInfo(teamName, gameInfo, week);
            auto newGame = std::make_shared<Game>(tokens, teamMapByAbbreviation);

            // Check if the game object already exists, if it does push existing object again
            int minScheduleIdx = std::min(newGame->getHomeTeam()->getSchedule(), newGame->getAwayTeam()->getSchedule());
            if (minScheduleIdx < NFLSchedule.size())
            {
                teamSchedule.push_back(NFLSchedule[minScheduleIdx][week]);
            }
            else
            {
                teamSchedule.push_back(newGame);
                // Update Elos if game was completed in csv file
                if (newGame->getIsComplete())
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
        auto team = std::make_shared<Team>(teamName, abbreviation, color, elo, city, latitude, longitude, lineNumber);

        // Add the team to both maps
        teamMapByAbbreviation[abbreviation] = team;
        teamMapByIndex[lineNumber] = team;

        // Add the team to the league structure by conference and division
        leagueStructure[conference][division].push_back(team); // This is where segmentation happens

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
        std::cout << std::left << std::setw(teamColumnWidth) << team->getName() << " | Elo: " << team->getElo() << " | Wins: " << team->getWinCount() << std::endl;
        std::cout << std::string(teamColumnWidth + weekColumnWidth + 3 + gameColumnWidth, '-') << std::endl;

        // Retrieve and print the games for the current team from the schedule
        const auto &games = NFLSchedule.at(i);

        int weekIndex = 0;

        for (const auto &game : games)
        {
            std::cout << std::left << std::setw(teamColumnWidth) << ("Week " + std::to_string(weekIndex))
                      << " | " << std::setw(gameColumnWidth) << game->printGame(team) << std::endl;
            ++weekIndex;
        }

        std::cout << std::endl;
    }
}

void NFLSim::printLeagueStructure() const
{
    for (const auto &conferencePair : leagueStructure)
    {
        const std::string &conference = conferencePair.first;
        std::cout << "Conference: " << conference << "\n";

        for (const auto &divisionPair : conferencePair.second)
        {
            const std::string &division = divisionPair.first;
            std::cout << "  Division: " << division << "\n";

            for (const auto &team : divisionPair.second)
            {
                std::cout << "    Team: " << team->getName() << " ("
                          << team->getAbbreviation() << "), Elo: "
                          << team->getWinCount() << "\n";
            }
        }
    }
}

void NFLSim::printPlayoffs() const
{
    std::cout << "Playoff Seeding:\n";

    // Iterate through each conference in the playoff seeding
    for (const auto &conferencePair : playoffSeeding)
    {
        const std::string &conference = conferencePair.first;
        const std::vector<std::shared_ptr<Team>> &teams = conferencePair.second;

        std::cout << "Conference: " << conference << "\n";

        // Print the teams in the playoff seeding
        for (size_t i = 0; i < teams.size(); ++i)
        {
            std::cout << "  Seed " << (i + 1) << ": "
                      << teams[i]->getName() << " ("
                      << teams[i]->getAbbreviation() << "), Win Count: "
                      << teams[i]->getWinCount() << "\n";
        }
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
double NFLSim::calculateFieldAdvantage(const City &homeCity, const City &awayCity)
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

    distance = distance / 1000 * 4 + 48; // 4 point advantage for every 1,0000 miles, 48 for home field

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
    if (NFLSchedule[homeTeamIdx][week]->getAwayTeam()->getName() == homeTeam.getName())
    {
        elo_diff += 25;
    }
    if (NFLSchedule[awayTeamIdx][week]->getAwayTeam()->getName() == awayTeam.getName())
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

void NFLSim::getHomeOddsStandard(std::shared_ptr<Game> &game)
{
    // Access the home and away teams via shared pointers
    auto homeTeam = game->getHomeTeam();
    auto awayTeam = game->getAwayTeam();

    // If bye week, skip odds calculation
    if (homeTeam->getName() == awayTeam->getName())
    {
        return;
    }

    double eloDiff = adjustEloForByes(*game, *homeTeam, *awayTeam);

    // If distance hasn't been calculated before, do it; else grab the value
    if (game->getFieldAdvantage() == -1)
    {
        double fieldAdvantage = calculateFieldAdvantage(homeTeam->getCity(), awayTeam->getCity());
        game->setFieldAdvantage(fieldAdvantage);
        eloDiff += fieldAdvantage;
    }
    else
    {
        eloDiff += game->getFieldAdvantage();
    }

    double homeOdds = calculateHomeOdds(eloDiff);
    game->setHomeOdds(homeOdds);
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
        for (auto &gamePtr : weeklySchedule)
        {
            // Call getHomeOddsStandard for each game that is not complete
            if (!gamePtr->getIsComplete())
            {
                getHomeOddsStandard(gamePtr); // Dereference shared_ptr to pass the object
            }
        }
    }
}

void NFLSim::processTeamGames(int teamIndex)
{
    // Get the specific team's weekly schedule using the provided index
    auto &weeklySchedule = NFLSchedule[teamIndex];

    // Iterate over each game in the team's weekly schedule
    for (auto &gamePtr : weeklySchedule)
    {
        // Call getHomeOddsStandard for each game that is not complete
        if (!gamePtr->getIsComplete())
        {
            getHomeOddsStandard(gamePtr); // Dereference shared_ptr to pass the object
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
    auto &gamePtr = NFLSchedule[scheduleIdx][week]; // std::shared_ptr<Game>
    auto &game = *gamePtr;                          // Dereference to get Game&

    // If game outcome has already been set, reset changes
    if (game.getEloEffect() != 0)
    {
        double eloChange = game.getEloEffect();
        game.getHomeTeam()->updateElo(-eloChange);
        game.getAwayTeam()->updateElo(eloChange);
    }

    // Set game scores
    game.setHomeTeamScore(homeScore);
    game.setAwayTeamScore(awayScore);
    game.setIsComplete(true);

    // Check for tie
    if (homeScore == awayScore)
    {
        // Handle a tie game
        game.getHomeTeam()->updateWinCount(0.5);
        game.getAwayTeam()->updateWinCount(0.5);
    }
    else
    {
        // Handle normal win/loss outcome
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

    // Process games for both teams after the update
    processTeamGames(game.getHomeTeam()->getSchedule());
    processTeamGames(game.getAwayTeam()->getSchedule());

    std::cout << "Game and Elo updated." << std::endl;
}

void NFLSim::updateEloRatings(std::shared_ptr<Game> gamePtr)
{
    const double K = 4.0;                   // K-factor
    const double MOV_MULTIPLIER_BASE = 2.2; // Base for margin-of-victory multiplier
    const double MOV_SCALE = 0.001;         // Scaling factor for Elo difference

    // Dereference to get Game&
    auto &game = *gamePtr;

    // Get home and away teams
    auto homeTeamPtr = game.getHomeTeam();
    auto awayTeamPtr = game.getAwayTeam();

    if (!homeTeamPtr || !awayTeamPtr)
    {
        std::cerr << "Error: Null pointer for home or away team." << std::endl;
        return;
    }

    // Access the Team objects through the shared pointers
    auto &homeTeam = *homeTeamPtr;
    auto &awayTeam = *awayTeamPtr;

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
    homeTeam.updateElo(homeEloAdjustment);
    awayTeam.updateElo(awayEloAdjustment);

    // Set the Elo effect for the game
    game.setEloEffect(homeEloAdjustment);
}

void NFLSim::simRegularSeason()
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
            if (game->getIsComplete())
                continue;

            // Generate a random float between 0 and 1
            float randNum = static_cast<float>(std::rand()) / RAND_MAX;

            // Generate scores using a log-linear distribution
            int score1 = static_cast<int>(3 + 30 * std::log(1.0f + std::rand() / (static_cast<float>(RAND_MAX) + 1.0f)));
            int score2 = static_cast<int>(3 + 30 * std::log(1.0f + std::rand() / (static_cast<float>(RAND_MAX) + 1.0f)));

            int winningScore, losingScore;
            std::shared_ptr<Team> winningTeam, losingTeam;

            // Determine if the game ends in a tie
            if (randNum < 0.01f)
            {
                game->setHomeTeamScore(score1);
                game->setAwayTeamScore(score1); // Both teams get the same score
                game->getHomeTeam()->updateWinCount(0.5);
                game->getAwayTeam()->updateWinCount(0.5);
            }
            else
            {
                // Ensure that one score is higher than the other
                winningScore = std::max(score1, score2);
                losingScore = std::min(score1, score2);
                if (winningScore == losingScore)
                    winningScore++; // Avoid ties unless specified

                // Determine the winning and losing team
                if (randNum > game->getHomeOdds())
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
            game->setIsComplete(true);
            updateEloRatings(game);

            // Process games for each team
            processTeamGames(game->getHomeTeam()->getSchedule());
            processTeamGames(game->getAwayTeam()->getSchedule());

            // Record the loss for the losing team
            if (winningTeam && losingTeam)
            {
                int pointDifferential = winningScore - losingScore;
                losingTeam->addLoss(winningTeam, pointDifferential);
            }
        }
    }

    // Determine division winners and print league structure
    getDivisionWinners();
    getWildCard();
    printPlayoffs();
}

void NFLSim::getDivisionWinners()
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

void NFLSim::getWildCard()
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

std::shared_ptr<Team> NFLSim::resolveTiebreaker(const std::shared_ptr<Team> &team1,
                                                const std::shared_ptr<Team> &team2)
{
    auto team1LostTo = team1->getTeamsLostTo();
    auto team2LostTo = team2->getTeamsLostTo();

    // Seed the random number generator if not already seeded
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    // Check if both teams have played each other
    auto team1LostToTeam2 = team1LostTo.find(team2);
    auto team2LostToTeam1 = team2LostTo.find(team1);

    if (team1LostToTeam2 != team1LostTo.end() &&
        team2LostToTeam1 != team2LostTo.end())
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