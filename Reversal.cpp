#include "Reversal.h"
#include <qeventloop.h>
#include <iostream>

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
	connect(rs, &QTcpSocket::connected, this, [&]
	{

			qDebug().nospace() << key << ": successfully connected to the remote";
			rs->write("connect " + key.toLocal8Bit() + ".");
			rs->flush();
			this->recv();
	});
	rs->connectToHost(rs_adr,rs_port);
	
}

void Reversal::recv()
{
		QEventLoop loop;
		rs->disconnect();
		obj = new QTcpSocket;
		connect(obj, &QTcpSocket::disconnected, &loop, [&]
		{
			loop.exit(false);
		});
		connect(obj, &QTcpSocket::readyRead, &loop, [&]
		{
			loop.exit(true);
		});
		connect(rs, &QTcpSocket::disconnected, &loop, [&]
		{
			loop.exit(false);
		});
		connect(rs, &QTcpSocket::readyRead, &loop, [&]
		{
			loop.exit(true);
		});
		connect(obj, &QTcpSocket::connected, &loop, [&]
		{
			qDebug().nospace() << key << ": successfully connected to the object";
		});
		obj->connectToHost(obj_adr, obj_port);
		obj->waitForConnected();
		QByteArray a;
		while(loop.exec())
		{
			rs->write(a=obj->readAll());
			//qDebug() << "c->s " << a;
			obj->write(a=rs->readAll());
			//qDebug() << "s->c " << a;
			rs->flush();
			obj->flush();
			std::cout << "+";
		}
		rs->disconnect();
		obj->disconnect();
		obj->disconnectFromHost();
		rs->disconnectFromHost();
}
