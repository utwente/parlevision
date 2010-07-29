#ifndef APPLICATION_H
#define APPLICATION_H

class QCoreApplication;

namespace plv
{
    class Application
    {
    public:
        Application(QCoreApplication* app);
        void init();

    private:
        void loadBuiltins();
        void loadPlugins();
        QCoreApplication* app;
    };
}

#endif // APPLICATION_H
