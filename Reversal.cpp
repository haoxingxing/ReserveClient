#include "Reversal.h"
#include <qeventloop.h>

Reversal::Reversal(QObject *parent, qintptr rs_port, qintptr obj_port, QString key,QString rs_adr,QString obj_adr)
	: QObject(parent),rs_port(rs_port),obj_port(obj_port),key(std::move(key)),rs_adr(rs_adr),obj_adr(obj_adr)
{
}

Reversal::~Reversal()
{
}

void Reversal::Run()
{
	rs = new QTcpSocket;
	connect(rs, &QTcpSocket::readyRead, this, &Reversal::recv);
	connect(rs, &QTcpSocket::connected, this, [&]
	{
			rs->write("connect " + key.toLocal8Bit() + ".");
			rs->flush();
	});
	rs->connectToHost(rs_adr,rs_port);
	
}

void Reversal::recv()
{
	static bool is_r = false;
	if(!is_r&&rs->canReadLine())
	{
		auto x = rs->readLine();
		qDebug() << x;
			is_r = true;
			QEventLoop loop;
			obj = new QTcpSocket;
			connect(obj, &QTcpSocket::disconnected, &loop, [&] {loop.exit(false); });
			connect(obj, &QTcpSocket::readyRead, &loop, [&] {loop.exit(true); });
			connect(rs, &QTcpSocket::disconnected, &loop, [&] {loop.exit(false); });
			connect(rs, &QTcpSocket::readyRead, &loop, [&] {loop.exit(true); });
			connect(obj, &QTcpSocket::connected, &loop, [&] {loop.exit(true); });
			obj->connectToHost(obj_adr, obj_port);
			while(loop.exec())
			{
				qDebug("+");
				rs->write(obj->readAll());
				obj->write(rs->readAll());
				rs->flush();
				obj->flush();
			}
			obj->disconnectFromHost();
			rs->disconnectFromHost();
	}
}
