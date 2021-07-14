#include <PlatformWin32.hh>
#include <Screen.h>

namespace jwm {

    class ScreenWin32: public Screen {
    public:
        explicit ScreenWin32(JNIEnv* env, class AppWin32& app);
        ~ScreenWin32() override;

    public:
        bool init();
        void close();

    public:
        int getX() const;
        int getY() const;
        int getWidth() const;
        int getHeight() const;
        float getScale() const;
        bool isPrimary() const;

    private:
        AppWin32& _app;
    };

}