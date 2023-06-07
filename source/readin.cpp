#include <basics.h>

namespace Map{
    int n, m;
    Matrix<char, 1, 1> charMap;
    Matrix<Tile::TileKind, 1, 1> tileMap;
    Matrix<int, 1, 1> lockMap;
};

namespace Game{
    int totTime;
    int randomizeSeed;
    int remainFrame, fund;
    int currentFrame;
    Ingredient ingrdList[INGRD_NR_MAX];
    Recipe recipList[RECIP_NR_MAX];
    Ordertemplate totodList[TOTOD_NR_MAX];
    Order orderList[ORDER_NR_MAX];
    Player playrList[PLAYR_NR_MAX];
    Entity enttyList[ENTTY_NR_MAX];
    int ingrdCnt, recipCnt, orderCnt, enttyCnt, playrCnt, totodCnt;

    Location ingrdDestination[INGRD_NR_MAX];
    Direction::DirectionKind ingrdDirection[INGRD_NR_MAX];
    Location washDestination, cleanDestination, surveDestination, dirtyDestination, plateDestination, indDestination;
    Direction::DirectionKind washDirection, cleanDirection, surveDirection, dirtyDirection, plateDirection, indDirection;

    std::vector<Direction::DirectionKind> plateDirectionList;
    std::vector<Location> plateDestinationList;
    std::set<int> readyPlates, absentPlates;
    int readyPlateCnt;
    int dirtyPlateCnt;
    int cleanPlateCnt;

    Location chopDestination, potDestination, panDestination;
    Direction::DirectionKind chopDirection, potDirection, panDirection;

    std::optional<Cooker::CookerKind> needToCook = std::nullopt;
    //recip_information
    std::unordered_map<std::string, int> madeFrom, madeFor;
    std::unordered_set<std::string> items;
    std::unordered_map<std::string, std::vector<int>> ingrdPlace;

    AttentionOrder attentionOrderList[ATTEN_ORDER_NR];
    bool attentionInitialized = false;
    int poolDirtyCnt = 0, serveDirtyCnt = 0;
    int panTime, potTime;
    int attentionMaxCnt;
}

void init_read()
{
    using namespace Game;
    std::string s;
    std::stringstream ss;

    std::getline(std::cin, s, '\0');
    ss << s;

    ss >> Map::m >> Map::n;
    Log("Get map N: %d M: %d", Map::n, Map::m);
    for (auto location : Map::tileMap) {
        char tilechar;
        ss >> tilechar;
        Map::tileMap[location] = Tile::decode(tilechar);
        assert(Map::tileMap[location] != Tile::None);
        switch (Map::tileMap[location])
        {
        case Tile::Floor:
            Path::abilityMap[location] = true;
            break;
        default:
            Path::abilityMap[location] = false;
            break;
        }
    }

    for (auto location : Map::tileMap)
        StaticPath::getSingleSourceBFS(location);
    
    for (auto location : Map::tileMap) {
        if (Map::tileMap[location] == Tile::Floor) {
            for (int i = 0; i < Direction::Direction_NR; i++) {
                Direction::DirectionKind direction = (Direction::DirectionKind) i;
                if (Direction::encode(direction).size() > 1)
                    continue;
                Location to = location[direction];
                if (!to.isvalid()) continue;
                if (Map::tileMap[to] == Tile::PlateReturn) dirtyDestination = location, dirtyDirection = direction;
                if (Map::tileMap[to] == Tile::PlateRack) cleanDestination = location, cleanDirection = direction;
                if (Map::tileMap[to] == Tile::Sink) washDestination = location, washDirection = direction;
                if (Map::tileMap[to] == Tile::ChoppingStation) chopDestination = location, chopDirection = direction;
                // if (location == Location(4, 8)) {
                //     Log("IN (4, 8)");
                //     Log("%d\n", (int) Map::tileMap[to]);
                //     Log("%c\n", Tile::encode(Map::tileMap[to]));
                // }
                if (Map::tileMap[to] == Tile::ServiceWindow) surveDestination = location, surveDirection = direction;
            }
        }
    }

    Log("SERVE AT (%d, %d)", surveDestination.x, surveDestination.y);

    /* INGREDIENT BOXES */
    ss >> ingrdCnt;
    for (int i = 0; i < ingrdCnt; i++) {
        ss >> s;
        assert(s == "IngredientBox");
        ss >> ingrdList[i];
        if (ingrdPlace.find(ingrdList[i].name) == ingrdPlace.end())
            ingrdPlace[ingrdList[i].name] = std::vector<int>();
        ingrdPlace[ingrdList[i].name].push_back(i);
        for (int j = 0; j < Direction::Direction_NR; j++) {
            Direction::DirectionKind direction = (Direction::DirectionKind) j;
            if (Direction::encode(direction).size() > 1)
                continue;
            Location to = ingrdList[i].location[direction];
            if (!to.isvalid()) continue;
            if (Map::tileMap[to] == Tile::Floor) {
                ingrdDestination[i] = to;
                ingrdDirection[i] = Direction::getrev(direction);
            }
        }
    }

    Log("END IRGBOX");

    /* RECIPES */
    ss >> recipCnt;
    for (int i = 0; i < recipCnt; i++) {
        ss >> recipList[i];
        //update recipe information into maps and sets
        items.insert(recipList[i].nameAfter);
        items.insert(recipList[i].nameBefore);
        madeFrom[recipList[i].nameAfter] = i;
        madeFor[recipList[i].nameBefore] = i;
    }

    Log("END REP");

    ss >> totTime >> randomizeSeed >> totodCnt;

    for (int i = 0; i < totodCnt; i++)
        ss >> totodList[i];
    
    Log("TOTOD INIT");

    /* PLAYERS */
    ss >> playrCnt;
    assert(playrCnt == 2);
    for (int i = 0; i < playrCnt; i++) {
        ss >> playrList[i].position;
        playrList[i].resume_time = 0;
        playrList[i].entity = std::nullopt;
    }

    Log("END PL");

    /* ENTITIES: PLATE POT PAN */
    ss >> enttyCnt;
    for (int i = 0; i < enttyCnt; i++) {
        ss >> enttyList[i].location;
        ss >> enttyList[i];
        /* PLATES */
        if (enttyList[i].containerKind == Container::Plate) {
            for (int j = 0; j < Direction::Direction_NR; j++) {
                Direction::DirectionKind direction = (Direction::DirectionKind) j;
                if (Direction::encode(direction).size() > 1)
                    continue;
                Location to = enttyList[i].location[direction];
                if (!to.isvalid()) continue;
                if (Map::tileMap[to] == Tile::Floor) {
                    plateDirection = Direction::getrev(direction);
                    plateDestination = to;
                    plateDirectionList.push_back(Direction::getrev(direction));
                    plateDestinationList.push_back(to);
                }
            }
        }
        if (enttyList[i].containerKind == Container::Pot) {
            for (int j = 0; j < Direction::Direction_NR; j++) {
                Direction::DirectionKind direction = (Direction::DirectionKind) j;
                if (Direction::encode(direction).size() > 1)
                    continue;
                Location to = enttyList[i].location[direction];
                if (!to.isvalid()) continue;
                if (Map::tileMap[to] == Tile::Floor) {
                    potDirection = Direction::getrev(direction);
                    potDestination = to;
                }
            }
        }
        if (enttyList[i].containerKind == Container::Pan) {
            for (int j = 0; j < Direction::Direction_NR; j++) {
                Direction::DirectionKind direction = (Direction::DirectionKind) j;
                if (Direction::encode(direction).size() > 1)
                    continue;
                Location to = enttyList[i].location[direction];
                if (!to.isvalid()) continue;
                if (Map::tileMap[to] == Tile::Floor) {
                    panDirection = Direction::getrev(direction);
                    panDestination = to;
                }
            }
        }
    }
    readyPlateCnt = plateDestinationList.size();
    assert(readyPlateCnt);
    for (int i = 0; i < readyPlateCnt; i++)
        readyPlates.insert(i);
    attentionMaxCnt = std::min(readyPlateCnt, ATTEN_ORDER_NR);
    Log("END INIT");
}

bool frame_read(int nowFrame)
{
    using namespace Game;
    std::string s;
    std::stringstream ss;
    std::getline(std::cin, s, '\0');
    ss.str(s);
    if (std::cin.rdbuf()->in_avail() > 0)
    {
        std::cerr << "Warning: skipping frame " << nowFrame
             << " to catch up with the game" << std::endl;
        return true;
    }
    ss >> s;
    assert(s == "Frame");
    ss >> currentFrame;
    assert(currentFrame == nowFrame);
    ss >> remainFrame >> fund;
    Log("frame remaining: %d", remainFrame);
    ss >> orderCnt;
    for (int i = 0; i < orderCnt; i++) {
        ss >> orderList[i];
        Log("%s", orderList[i].requirement[0].c_str());
    }
    
    //Cold Start
    if (!attentionInitialized) {
        for (int i = 0; i < attentionMaxCnt; i++) {
            attentionOrderList[i] = orderList[i];
        }
    }

    ss >> playrCnt;
    for (int i = 0; i < playrCnt; i++) {
        ss >> playrList[i];
        Log("player (%.2lf %.2lf)", playrList[i].position.x, playrList[i].position.y);
    }

    ss >> enttyCnt;
    for (int i = 0; i < enttyCnt; i++) {
        ss >> enttyList[i].location;
        ss >> enttyList[i];
        if (enttyList[i].containerKind == Container::DirtyPlates) {
            if (enttyList[i].location == dirtyDestination[dirtyDirection]) {
                serveDirtyCnt += enttyList[i].sum;
            } else if (enttyList[i].location == washDestination[washDirection]) {
                poolDirtyCnt += enttyList[i].sum;
            }
        }
        if (enttyList[i].containerKind == Container::Pan) {
            // assert(~enttyList[i].currentFrame);
            // assert(~enttyList[i].totalFrame);
            Game::panTime = enttyList[i].currentFrame - enttyList[i].totalFrame;
        }
        if (enttyList[i].containerKind == Container::Pot) {
            // assert(~enttyList[i].currentFrame);
            // assert(~enttyList[i].totalFrame);
            Game::potTime = enttyList[i].currentFrame - enttyList[i].totalFrame;
        }
    }
    return false;
}
