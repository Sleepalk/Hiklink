#pragma once

#include <QWidget>
#include "ui_SendFile.h"
#include "basicwindow.h"
class SendFile : public BasicWindow
{
	Q_OBJECT

public:
	SendFile(QWidget *parent = nullptr);
	~SendFile();
private slots:
	void on_sendBtn_clicked();
	void on_openBtn_clicked();
signals:
	void sendFileClicked(QString& strData, int& msgType, QString fileName);
private:
	Ui::SendFileClass ui;
	QString m_filePath;
};
