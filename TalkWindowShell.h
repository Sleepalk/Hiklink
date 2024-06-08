#pragma once

#include <QWidget>
#include "basicwindow.h"
#include "ui_TalkWindowShell.h"
#include<qlistwidget.h>
#include<qmap.h>
#include"CommonUtils.h"
#include"EmotionWindow.h"
#include"TalkWindow.h"
#include<qtcpsocket.h>
#include<qudpsocket.h>
class TalkWindow;
class TalkWindowItem;
class EmotionWindow;

enum GroupType {
	COMPANY = 0,//公司群
	PERSONELGROUP,//人事部
	DEVELOPMENTGROUP,//研发部
	MARKETGROUP,//市场部
	PTOP//同事之间单独聊天
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = nullptr);
	~TalkWindowShell();
private:
	void initControl();//初始化部件
	void initTcpSocket();//初始化tcp客户端
	void initUdpSocket();//初始化udpSocket
	void getEmployeesID(QStringList& employeesIDList);//获取所有员工qq号
	bool createJSFile(QStringList &employeesList);//创建js文件(写js)
	void handleReceiveMsg(int senderEmployeeID, int msgType, QString strMsg);//更新接收消息到view
public:
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid/*, GroupType grouptype*/);//添加新的聊天窗口

	void setCurrentWidget(QWidget* widget);//设置当前聊天窗口

	QMap<QListWidgetItem*, QWidget*> getTalkWindowItemMap()const;

public slots:
	void onEmotionBtnClicked(bool);//表情按钮点击后执行的槽函数
	//参数(数据，数据类型，文件)
	void updateSendTcpMsg(QString& strData, int& msgType, QString fileName = "");
private slots:
	void onTalkWindowItemClicked(QListWidgetItem* item);//聊天窗口左侧列表项被点击时的槽函数

	void onEmotionItemClicked(int emotionNum);//表情点击时槽函数

	void processPendingData();//处理udp广播收到的数据

private:
	Ui::TalkWindowShellClass ui;
	QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;//当前打开的聊天窗口映射
	EmotionWindow* m_emotionWindow;//表情窗口
private:
	QTcpSocket* m_tcpClientSocket;//客户端tcp
	QUdpSocket* m_udpReceiver;//udp接收端
};
