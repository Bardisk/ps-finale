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
  Order totodList[TOTOD_NR_MAX], orderList[ORDER_NR_MAX];
  Player playrList[PLAYR_NR_MAX];
  Entity enttyList[ENTTY_NR_MAX];
  int ingrdCnt, recipCnt, orderCnt, enttyCnt, playrCnt, totodCnt;
}

void init_read()
{
    std::string s;
    std::stringstream ss;

    std::getline(std::cin, s, '\0');
    ss << s;

    ss >> Map::m >> Map::n;
    Log("Get map N: %d M: %d", Map::n, Map::m);
    for (auto tile : Map::tileMap) {
        char tilechar;
        ss >> tilechar;
        tile = Tile::decode(tilechar);
    }

    /* 读入原料箱：位置、名字、以及采购单价 */
    ss >> Game::ingrdCnt;
    for (int i = 0; i < Game::ingrdCnt; i++) {
        ss >> s;
        assert(s == "IngredientBox");
        ss >> Game::ingrdList[i];
    }

    /* 读入配方：加工时间、加工前的字符串表示、加工容器、加工后的字符串表示 */
    ss >> Game::recipCnt;
    for (int i = 0; i < Game::recipCnt; i++)
        ss >> Game::recipList[i];

    /* 读入总帧数、当前采用的随机种子、一共可能出现的订单数量 */
    ss >> Game::totTime >> Game::randomizeSeed >> Game::totodCnt;

    /* 读入订单的有效帧数、价格、权重、订单组成 */
    for (int i = 0; i < Game::totodCnt; i++)
        ss >> Game::totodList[i];

    /* 读入玩家信息：初始坐标 */
    ss >> Game::playrCnt;
    assert(Game::playrCnt == 2);
    for (int i = 0; i < Game::playrCnt; i++) {
        ss >> Game::playrList[i].position;
        Game::playrList[i].resume_time = 0;
        Game::playrList[i].entity = std::nullopt;
    }

    /* 读入实体信息：坐标、实体组成 */
    ss >> Game::enttyCnt;
    for (int i = 0; i < Game::enttyCnt; i++) {
        ss >> Game::enttyList[i].position >> s;
        Game::enttyList[i].containerKind = Container::decode(s);
        Game::enttyList[i].sum = 1;
    }
}

bool frame_read(int nowFrame)
{
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
    ss >> Game::currentFrame;
    assert(Game::currentFrame == nowFrame);
    ss >> Game::remainFrame >> Game::fund;
    /* 读入当前的订单剩余帧数、价格、以及配方 */
    ss >> Game::orderCnt;
    for (int i = 0; i < Game::orderCnt; i++)
        ss >> Game::orderList[i];
    /* 读入玩家坐标、x方向速度、y方向速度、剩余复活时间 */
    for (int i = 0; i < Game::playrCnt; i++)
        ss >> Game::playrList[i];

    ss >> Game::enttyCnt;
    /* 读入实体坐标 */
    for (int i = 0; i < Game::enttyCnt; i++)
        ss >> Game::enttyList[i];
    return false;
}