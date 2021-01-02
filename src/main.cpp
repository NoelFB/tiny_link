#include <blah.h>

using namespace Blah;

namespace
{

}

int main()
{
	Config config;
	config.width = 1280;
	config.height = 720;
	config.name = "tiny link";
	
	App::run(&config);
}