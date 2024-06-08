#include "ReceiveFile.h"
#include<qfiledialog.h>
#include<qmessagebox.h>
extern QString gfileName;
extern QString gfileData;
ReceiveFile::ReceiveFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);//窗口关闭时资源回收
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ReceiveFile");
	this->move(500, 300);
	connect(_titleBar, &TitleBar::signalButtonCloseClicked, this, &ReceiveFile::refuseFile);//信号与信号的连接
}

ReceiveFile::~ReceiveFile()
{}

void ReceiveFile::setMsg(QString & msgLabel)
{
	ui.label->setText(msgLabel);
}

void ReceiveFile::on_canelBtn_clicked()
{
	emit refuseFile();
	this->close();
}

void ReceiveFile::on_okBtn_clicked()
{
	this->close();
	//获取想要保存的文件路径
	QString fileDirPath = QFileDialog::getExistingDirectory(NULL, QString("文件保存路径"),
		"/");
	QString filePath = fileDirPath + "/" + gfileName;
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(nullptr, QString("提示"), QString("文件接收失败"));
	}
	else
	{
		file.write(gfileData.toUtf8());
		file.close();
		QMessageBox::information(nullptr, QString("提示"), QString("文件接收成功"));
	}
}

