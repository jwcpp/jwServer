#include "GameServer.h"
#include "Dumper.h"
#include "Log.h"
#include "ColorConsoleAppender.h"
#include "FileAppender.h"

Dumper gDumper;
GameServer gGameserver;
Logger<0> gLog;

int main(int argc, char* argv[])
{
	asio::io_context io_;
	gLog.init(&io_);

	Util::mkdir("log");

	std::shared_ptr<ColorConsoleAppender> sh_console = std::make_shared<ColorConsoleAppender>();
	std::shared_ptr<FileAppender> sh_file = std::make_shared<FileAppender>("./log/gameserver", 1024 * 1024 * 50);

	gLog.addAppender(sh_console.get());
	gLog.addAppender(sh_file.get());

	std::thread thread_([&io_]() { io_.run(); });
	asio::io_context::work worker(io_); // No event not exit

	int ret = 0;
	if (ret = gGameserver.init(argc, argv))
	{
		PRINT_ERROR("server init() fail: %d", ret);
	}
	else
	{
		PRINT_INFO("server init complete!");
		gGameserver.run(10);
	}
	gGameserver.stop();
	gGameserver.release();

	system("timeout 1");
	io_.stop();
	thread_.join();
	return 0;
}