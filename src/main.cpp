#include "interface/rest/controller.h"
#include "interface/rest/AppComponent.h"
#include "agents/robin.h"

#include <oatpp/network/Server.hpp>

#include <iostream>

using namespace rest;

void run() {
	oatpp::network::Server* server_ptr;
	std::thread oatppThread([&server_ptr] {
		AppComponent components;
		OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

		agents::Robin robin;
		auto controller = std::make_shared<rest::Controller>(robin);
		router->addController(controller);

		oatpp::network::Server server(connectionProvider, connectionHandler);
		std::cout << "Server is now listening on port " << (char*)connectionProvider->getProperty("port").getData() << std::endl;
		
		server_ptr = &server;
		server.run();
	});

	while (true) {
		std::cout << "[This is were a backend could be added]" << std::endl;
		std::cin.ignore();
	}

	server_ptr->stop();
	oatppThread.join();
}

int main(int argc, const char* argv[]) {
	oatpp::base::Environment::init();

	run();
	
	/* Print how much objects were created during app running, and what have left-probably leaked */
	/* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
	std::cout << "\nEnvironment:\n";
	std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
	std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";
	
	oatpp::base::Environment::destroy();
	return 0;
}