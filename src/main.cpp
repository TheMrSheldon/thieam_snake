#include "interface/rest/controller.h"
#include "interface/rest/AppComponent.h"
#include "agents/robin.h"

#include <oatpp/network/Server.hpp>

#include <iostream>

using namespace rest;

void myBackendLogicDummy() {
	while (true) {
		OATPP_LOGI("MyBackend", "Press enter to continue the loop, Strg+C to force unclean stop");
		std::cin.ignore();
	}
}

void run() {
	std::thread oatppThread([] {
		/* Register Components in scope of thread method */
		AppComponent components;
		/* Get router component */
		OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
		/* Get connection handler component */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);
		/* Get connection provider component */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

		/* Create MyController and add all of its endpoints to router */
		agents::Robin robin;
		auto controller = std::make_shared<rest::Controller>(robin);
		router->addController(controller);

		/* Create server which takes provided TCP connections and passes them to HTTP connection handler */
		oatpp::network::Server server(connectionProvider, connectionHandler);
		/* Print info about server port */
		OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

		/* Run server */
		server.run();
	});

	/* ToDo: Call your logic here! We are just calling some blocking dummy logic here */
	myBackendLogicDummy();
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