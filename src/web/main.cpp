#include "WebServer.h"
#include "Minidump/dumper.h"
#include "Log.h"
#include "ColorConsoleAppender.h"
#include "FileAppender.h"

Dumper gDumper;
WebServer gWebserver;
Logger<0> gLog;

int main()
{
	asio::io_context io_;
	gLog.init(&io_);

	Util::mkdir("log");

	std::shared_ptr<ColorConsoleAppender> sh_console = std::make_shared<ColorConsoleAppender>();
	std::shared_ptr<FileAppender> sh_file = std::make_shared<FileAppender>("./log/webserver", 1024 * 1024 * 50);

	gLog.addAppender(sh_console.get());
	gLog.addAppender(sh_file.get());

	std::thread thread_([&io_]() { io_.run(); });
	asio::io_context::work worker(io_); // No event not exit

	if (gWebserver.init())
	{
		PRINT_ERROR("server init() fail!");
	}
	else
	{
		PRINT_INFO("server init complete!");
		gWebserver.run(10);
	}
	gWebserver.stop();
	gWebserver.release();

	system("timeout 1");
	io_.stop();
	thread_.join();
	return 0;
}