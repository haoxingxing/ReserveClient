#pragma once

#include <QObject>
#include <QTcpSocket>
class Reversal : public QObject
{
	Q_OBJECT

public:
	Reversal(QObject *parent,qintptr rs_port,qintptr obj_port,QString key,QString rs_adr,QString obj_adr);
	~Reversal();
	void Run();
	void recv();

private:
	QTcpSocket* rs;
	QTcpSocket* obj;
	qintptr rs_port;
	qintptr obj_port;
	QString key, rs_adr,obj_adr;
};
