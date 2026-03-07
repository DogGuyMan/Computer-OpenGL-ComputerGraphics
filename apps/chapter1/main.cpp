#include "entry.h"

int main()
{
	SJH::Chapter1::my_application app;

	if (!app.init())
		return -1;

	app.run();
	app.shutdown();

	return 0;
}
