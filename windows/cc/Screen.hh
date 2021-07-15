#include <PlatformWin32.hh>

namespace jwm {

    class Screen {
    public:
        HMONITOR hMonitor;
        int x;
        int y;
        int width;
        int height;
        float scale;
        bool isPrimary;
    };

}