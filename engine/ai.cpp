#include "ai.h"
#include "log.h"
#include "util.h"

ControllerFactory::~ControllerFactory()
{
	typedef std::pair<const int, Controller*> KeyValue;
	foreach(KeyValue & key_value, controllers) {
		delete key_value.second;
	}
}

void ControllerFactory::add_controller(int ai, Controller * controller)
{
	controllers[ai] = controller;
}

Controller * ControllerFactory::get_controller(int ai) const
{
	std::map<int, Controller*>::const_iterator result = controllers.find(ai);
	if(result != controllers.end()) {
		return result->second;
	}
	log("Unknown AI code: {0}", ai);
	return 0;
}

