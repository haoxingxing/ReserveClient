#pragma once

#include <QObject>
#include <QMap>
#include <QTcpSocket>
#include <QQueue>
class Controller : public QObject
{
	Q_OBJECT

public:
	Controller(QObject *parent,QString remote,qintptr remote_port);
	void Run();
	~Controller();
	void addNode(QString object, qintptr port_obj,qintptr port_remote);
signals:
	void error(QString);
	void connected();
private:
	void receive();
	void send(QString);
	QString remote;
	qintptr remote_port;
	QQueue<qintptr> waiting_rp;
	QQueue<QPair<QString, qintptr>> waiting;
	QMap<QString, QPair<QString, qintptr>> pool;
	QTcpSocket r;
};
