#include <iostream>
#include <neo4j-cpp/neo4j-cpp.h>

int main(int argc, const char** argv) {
    if(argc < 2) {
        std::cerr << argv[0] << " <host>" << std::endl;
        return -1;
    }
    
    neo4j::client::init();
    auto con = neo4j::client::connect(argv[1], neo4j::connect_flags::insecure);
    std::cout << "Username: " << con->get_username() << "\n";
    std::cout << "Hostname: " << con->get_hostname() << "\n";
    std::cout << "Port    : " << con->get_port() << "\n";
    std::cout << "ServerID: " << con->get_server_id() << "\n";
    std::cout << "Secure  : " << (con->is_secure()?"true":"false") << "\n";
    std::cout << "CredExp : " << (con->is_credentials_expired()?"true":"false") << "\n";
    std::cout.flush();

    auto stream = con->run("MATCH path=shortestPath((:Person{name:\"James Thompson\"})-[:FOLLOWS*]-(:Person{name:\"Angela Scope\"})) RETURN path;");
    if(stream->check_failure()) {
        std::cerr << "Failed to execute statement:\n";
        std::cerr << stream->failure_details().message << "\n";
        std::cerr.flush();
    }
    auto res = stream->fetch_next();
    while(res) {
        std::cout << res.field(0).dump() << std::endl;
        
        res = stream->fetch_next();
    }
}