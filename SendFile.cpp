#include "SendFile.h"
#include <qfiledialog.h>
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include<qfile.h>
#include<qfileinfo.h>
#include<qmessagebox.h>
SendFile::SendFile(QWidget *parent)
	: BasicWindow(parent)
	,m_filePath("")
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);//窗口关闭时资源回收
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("SendFile");
	this->move(500, 300);
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &SendFile::sendFileClicked, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);
}

SendFile::~SendFile()
{}

void SendFile::on_openBtn_clicked()
{
	m_filePath = QFileDialog::getOpenFileName(this, QString("选择文件"), "/",
		"*.txt;;*.doc;;*.*");
	ui.lineEdit->setText(m_filePath);
}


void SendFile::on_sendBtn_clicked()
{
	if (!m_filePath.isEmpty())
	{
		QFile file(m_filePath);
		if (file.open(QIODevice::ReadOnly)) {
			QString str = file.readAll();
			int msgType = 2;//文件类型
			//文件名称
			QFileInfo fileInfo(m_filePath);
			QString fileName = fileInfo.fileName();
			emit sendFileClicked(str, msgType, fileName);
			file.close();
		}
	}
	else {
		QMessageBox::information(this, QString("提示"), QString("发送文件 %1 失败").arg(m_filePath));
		m_filePath = "";
		ui.lineEdit->clear();
		this->close();
		return;
	}
	m_filePath = "";
	close();
}
