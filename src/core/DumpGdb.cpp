#include "dumper.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sighandler(int sig);

static bool isDumping = false;
DumperHandler Dumper::dumperHandler;

Dumper::Dumper() {
	dumperHandler = NULL;

	struct rlimit rlim;
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &rlim);

	signal(SIGSEGV, &sighandler);
	signal(SIGABRT, &sighandler);
	signal(SIGFPE, &sighandler);
	signal(SIGPIPE, &sighandler);
}

void sighandler(int sig)
{
	if (isDumping)
	{
		// Crushing again, exiting...
		exit(0);
		return;
	}
	isDumping = true;

	signal(SIGSEGV, SIG_DFL);
	signal(SIGABRT, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGPIPE, SIG_DFL);

	// 调用上层处理器
	DumperHandler dumperHandler = Dumper::getDumperHandler();
	if (dumperHandler) {
		(*dumperHandler)();
	}

	raise(sig);
}
