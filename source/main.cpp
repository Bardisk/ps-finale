#include <basics.h>
#define OLD

int main()
{
    //c++ iostream is just rubbish
    std::ios::sync_with_stdio(false);
    std::cerr.tie(nullptr);
    std::cerr << std::nounitbuf;
    std::string s;
    std::stringstream ss;

    Log("Begin INIT");

    init_read();

    int totalFrame = 14400;
    for (int i = 0; i < totalFrame; i++) {
        bool skip = frame_read(i);
        if (skip) continue;

        Mainctr::getDecision();
        /* 输出当前帧的操作，此处仅作示例 */
        std::cout << "Frame " << i << "\n";

#ifdef OLD
        std::string response = Mainctr::respond();
#else
        std::string response = GameCtr::respond();
#endif

        std::cout << response;

        // flush the output stream
        std::cout.flush();
    }
}
