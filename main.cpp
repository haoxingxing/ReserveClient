#include <QCoreApplication>
#include <QTcpSocket>
#include "Reversal.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	QTcpSocket socket;
	socket.connectToHost("localhost", 8888);
	socket.waitForConnected();
	socket.write("create tcp 2200.");
	socket.flush();
	socket.waitForReadyRead();
	auto x =socket.readLine().split(' ');
	Reversal zx(nullptr, 8888, 22, x[1], "localhost", "192.168.1.100");
	zx.Run();
    return a.exec();
}
