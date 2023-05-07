#include <basics.h>

int main()
{
    //c++ iostream is just rubbish
    std::ios::sync_with_stdio(false);
    std::cerr.tie(nullptr);
    std::cerr << std::nounitbuf;
    std::string s;
    std::stringstream ss;
    int frame;

    init_read();

    int totalFrame = 14400;
    for (int i = 0; i < totalFrame; i++) {
        bool skip = frame_read(i);
        if (skip) continue;

        Mainctr::getDecision();
        /* 输出当前帧的操作，此处仅作示例 */
        std::cout << "Frame " << i << "\n";

        std::string response = Mainctr::respond();
        std::cout << response;

        // flush the output stream
        std::cout.flush();
    }
}
