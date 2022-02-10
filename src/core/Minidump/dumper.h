#ifndef DUMPER_H
#define DUMPER_H
typedef void (*DumperHandler)();

class Dumper {
	static DumperHandler dumperHandler;
public:
	Dumper();

	static void setDumperHandler(DumperHandler val) {
		dumperHandler = val;
	}
	static DumperHandler getDumperHandler() {
		return dumperHandler;
	}
};
#endif


