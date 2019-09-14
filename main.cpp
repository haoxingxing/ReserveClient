#include <QCoreApplication>
#include <QTcpSocket>
#include "Reversal.h"
#include "Controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	Controller c(nullptr,"localhost",8888);
	Controller::connect(&c, &Controller::connected, &c, [&] {c.addNode("192.168.1.100", 22, 223); });
	c.Run();
    return a.exec();
}
