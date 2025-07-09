#include <Sockum.hpp>

#include <thread>
#include <atomic>

int main()
{
    SockumClient *c2 = new SockumClient();

    std::thread t([c2]() { c2->run(); });
    t.detach();


    std::string path = "YOUR_FILE_PATH";

    c2->routeFile("file", path);

    std::cout << "Exiting...\n";
    c2->disconnect();
    delete c2;
    return 0;
}
