#include <basics.h>

namespace Map{
    int n, m;
    Matrix<char, 1, 1> charMap;
    Matrix<Tile::TileKind, 1, 1> tileMap;
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

    Location chopLocation, potLocation, panLocation;
    Direction::DirectionKind chopDirection, potDirection, panDirection;

    std::optional<Cooker::CookerKind> needToCook = std::nullopt;
    //recip_information
    std::unordered_map<std::string, int> madeFrom, madeFor;
    std::unordered_set<std::string> items;
    std::unordered_map<std::string, std::vector<int>> ingrdPlace;
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
                if (location == Location(4, 8)) {
                    Log("IN (4, 8)");
                    Log("%d\n", (int) Map::tileMap[to]);
                    Log("%c\n", Tile::encode(Map::tileMap[to]));
                }
                if (Map::tileMap[to] == Tile::ServiceWindow) surveDestination = location, surveDirection = direction;
            }
        }
    }

    Log("SERVE AT (%d, %d)", surveDestination.x, surveDestination.y);

    /* 读入原料箱：位置、名字、以及采购单价 */
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

    /* 读入配方：加工时间、加工前的字符串表示、加工容器、加工后的字符串表示 */
    ss >> recipCnt;
    for (int i = 0; i < recipCnt; i++) {
        ss >> recipList[i];
        //update recipe information into maps and sets
        items.insert(recipList[i].nameAfter);
        items.insert(recipList[i].nameBefore);
        madeFrom[recipList[i].nameAfter] = i;
        madeFor[recipList[i].nameBefore] = i;
    }

    /* 读入总帧数、当前采用的随机种子、一共可能出现的订单数量 */
    ss >> totTime >> randomizeSeed >> totodCnt;

    /* 读入订单的有效帧数、价格、权重、订单组成 */
    for (int i = 0; i < totodCnt; i++)
        ss >> totodList[i];
    
    //will be neglected
    for (int i = 0; i < ingrdCnt; i++) {
        //no need to cook
        if (ingrdList[i].name == totodList[0].requirement[0]) {
            needToCook = Cooker::None;
            indDestination = ingrdDestination[i];
            indDirection = ingrdDirection[i]; 
        }
    }
    if (!needToCook.has_value()) {
        std::string targetName = totodList[1].requirement[0];
        //should be able to be made from
        assert(madeFrom.find(targetName) != madeFrom.end());
        
        std::string prepareName = recipList[madeFrom[targetName]].nameBefore;
        Log("Target Name: %s Prepare Name: %s", targetName.c_str(), prepareName.c_str());
        needToCook = recipList[madeFrom[targetName]].kind;

        //should have at least an corresponding ingredient box
        assert(ingrdPlace[prepareName].size() > 0);
        //choose randomly (0)
        indDestination = ingrdDestination[ingrdPlace[prepareName][0]];
        indDirection = ingrdDirection[ingrdPlace[prepareName][0]];
    }

    /* 读入玩家信息：初始坐标 */
    ss >> playrCnt;
    assert(playrCnt == 2);
    for (int i = 0; i < playrCnt; i++) {
        ss >> playrList[i].position;
        playrList[i].resume_time = 0;
        playrList[i].entity = std::nullopt;
    }

    /* 读入实体信息：坐标、实体组成 */
    ss >> enttyCnt;
    for (int i = 0; i < enttyCnt; i++) {
        ss >> enttyList[i].location;
        ss >> enttyList[i];
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
                }
            }
        }
    }
}

bool frame_read(int nowFrame)
{
    using namespace Game;
    std::string s;
    std::stringstream ss;
    std::getline(std::cin, s, '\0');
    ss.str(s);
    /*
      如果输入流中还有数据，说明游戏已经在请求下一帧了
      这时候我们应该跳过当前帧，以便能够及时响应游戏。
    */
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
    /* 读入当前的订单剩余帧数、价格、以及配方 */
    ss >> orderCnt;
    for (int i = 0; i < orderCnt; i++) {
        ss >> orderList[i];
        Log("%s", orderList[i].requirement[0].c_str());
    }
    /* 读入玩家坐标、x方向速度、y方向速度、剩余复活时间 */
    ss >> playrCnt;
    for (int i = 0; i < playrCnt; i++) {
        ss >> playrList[i];
        Log("player (%.2lf %.2lf)", playrList[i].position.x, playrList[i].position.y);
    }

    ss >> enttyCnt;
    for (int i = 0; i < enttyCnt; i++) {
        ss >> enttyList[i].location;
        ss >> enttyList[i];
    }
    return false;
}