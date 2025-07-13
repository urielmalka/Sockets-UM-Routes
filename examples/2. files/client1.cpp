#include <Sockum.hpp>

#include <thread>
#include <atomic>

int main()
{
    SockumClient *c1 = new SockumClient();

    c1->setLogActivated(false); // Enable logging for debugging

    std::thread t([c1]() { c1->run(); });
    t.detach();

    c1->addFileRoute("file", "files/", [](std::map<std::string, std::any> args) {
        // Handle file route
        // This function will be called when a file is received
        std::string filename = std::any_cast<std::string>(args["filename"]);
        std::string finished = std::any_cast<std::string>(args["finished"]);

        size_t size = std::stoull(std::any_cast<std::string>(args["size"]));
        size_t load = std::stoull(std::any_cast<std::string>(args["load"]));


        double progress = (double)load / size * 100.0;

        if (finished == "true") {
            std::cout << "✅ File finalized: " << filename << std::endl;
        } else {
            std::cout << "⏳ File: " << filename << " - Progress: " << std::fixed << std::setprecision(2) << progress << "%" << std::endl;
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
