#include "Controller.h"
#include "Reversal.h"
#include <QThread>
Controller::Controller(QObject *parent, QString remote, qintptr remote_port)
	: QObject(parent),remote(remote),remote_port(remote_port)
{
	connect(&r, &QTcpSocket::readyRead, this, &Controller::receive);
	connect(&r, &QTcpSocket::connected, this, [&]
	{
			qDebug() << "successfully connected to the remote server";
			emit connected();
	});
	connect(&r, &QTcpSocket::disconnected, this, [&]
	{
			qDebug() << "remote server disconnected,reconnecting";
			this->Run();
	});
}

void Controller::Run()
{
	qDebug() << "connecting";
	r.connectToHost(remote, remote_port);
}

Controller::~Controller()
{
}

void Controller::addNode(QString object, qintptr port_obj, qintptr port_remote)
{
	if (r.isWritable()) {
		send("create tcp " + QString::number(port_remote));
		waiting.enqueue(QPair<QString, qintptr>(object, port_obj));
		waiting_rp.enqueue(port_remote);
		qDebug() << "requested,waiting";
	}
	else
	{
		qDebug() << "failed adding nodes: not connected to server";
	}
}

void Controller::receive()
{
	while (r.canReadLine())
	{
		auto recv = r.readLine();
		recv = recv.simplified();
		qDebug() << "received " << recv;
		auto d = recv.split(' ');
		if (d.empty())
			qDebug() << "err";
		if (d[0].size()==10)
		{
			if (d[1] == "success")
			{
				qDebug().nospace() << d[0] << ": successfully created socket";
			}
			else if (d[1] == "failed")
			{
				QString s;
				for (int i = 2; i < d.size(); i++)
					s.append(d[i] + " ");
				qDebug().nospace() << d[0] << ": failed creating socket:" << s;
			}
			else if (d[1] == "need_connector")
			{
				qDebug().nospace() << d[0] << ": a socket was requested to open locally";
				auto t = new QThread;
				auto x =new Reversal(nullptr,remote_port,pool[d[0]].second,d[0],remote, pool[d[0]].first);
				connect(t, &QThread::started, x, &Reversal::Run);
				x->moveToThread(t);
				t->start();
			}
		}
		else
		if (d[0] == "success")
		{
			auto n = waiting.dequeue();
			qDebug().nospace() << "successfully added node " << n << " => " << remote <<":"<< waiting_rp.dequeue();
			pool.insert(d[1], n);
		}
	
	}
}

void Controller::send(QString s)
{
	r.write(s.toLocal8Bit() + ".");
	qDebug() << "sending " << s;
}
