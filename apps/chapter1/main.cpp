#include "entry.h"

int main(int argc, char *argv[])
{
	SJH::Chapter1::my_application app;

	if (!app.init(argc, argv))
		return -1;

	app.run();

	return 0;
}
