#include <Sockum.hpp>

#include <thread>
#include <atomic>

int main()
{
    SockumClient *c1 = new SockumClient();

    std::thread t([c1]() { c1->run(); });
    t.detach();

    c1->addFileRoute("file", "files/", [](map<string, any> args) {
        // Handle file route
        // This function will be called when a file is received
        std::string filename = any_cast<std::string>(args["filename"]);
        std::string finished = any_cast<std::string>(args["finished"]);

        if (finished == "true") {
            std::cout << "File finalized: " << filename << std::endl;
        }else {
            std::cout << "File not finalized: " << filename << std::endl;
        }

    });

    std::string input;
    
    while (true) {
        std::getline(std::cin, input);
        if (input == "Q" || input == "q") {
            break;
        }
    }

    std::cout << "Exiting...\n";
    c1->disconnect();
    delete c1;
    return 0;
}
