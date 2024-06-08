#include "TalkWindowShell.h"
#include "basicwindow.h"
#include "CommonUtils.h"
#include "TalkWindow.h"
#include<qwidget.h>
#include"TalkWindowItem.h"
#include<qsqlquerymodel.h>
#include<qsqlquery.h>
#include<qmessagebox.h>
#include<qfile.h>
#include"WindowManager.h"
#include"QMsgTextEdit.h"
#include"ReceiveFile.h"
#include"SendFile.h"
QString gfileName;
QString gfileData;
const int gtcpPort = 8888;//服务端的tcp端口号
const int udpPort = 6666;//udp端口号
extern QString gLoginEmployeeID;//外部声明,UserLogin.cpp,当前登录者的qq号
TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();
	initUdpSocket();
	QFile file(":/Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (!file.size())//如果js文件里面的数据是空的
	{
		QStringList employeesIDList;
		getEmployeesID(employeesIDList);
		if (!createJSFile(employeesIDList))//调用
		{
			QMessageBox::information(this, QString("提示"), QString("更新js文件数据失败"));
		}
	}
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow = nullptr;
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	this->setWindowTitle(QString("聊天窗口"));
	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();//表情窗口隐藏
	QList<int> leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize);//分离器设置尺寸

	ui.listWidget->setStyle(new CustomProxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));

}

//初始化tcp客户端
void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);
}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceiver = new QUdpSocket(this);
	for (quint16 port = udpPort; port < udpPort + 200; port++)
	{
		if (m_udpReceiver->bind(port, QAbstractSocket::ShareAddress))//绑定一个udp端口
			break;
	}
	//消息来之后就对信息进行处理
	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

void TalkWindowShell::getEmployeesID(QStringList& employeesIDList)
{
	//QStringList employeesIDList;
	QSqlQueryModel queryModel;
	queryModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
	int employeesNum = queryModel.rowCount();//员工行数/员工个数
	QModelIndex index;
	for (int i = 0; i < employeesNum; ++i) {
		index = queryModel.index(i,0);
		employeesIDList << queryModel.data(index).toString();
	}
}

//创建js文件(写js)
bool TalkWindowShell::createJSFile(QStringList& employeesList)
{
	//读取txt文件数据
	QString strFileTxt = "Resources/MainWindow/MsgHtml/msgtmpl.txt";
	QFile fileRead(strFileTxt);
	QString strFile;//保存读到的数据
	if (fileRead.open(QIODevice::ReadOnly)) {
		strFile = fileRead.readAll();
		fileRead.close();
	}
	else {
		QMessageBox::information(this, QString("提示"), QString("读取msgtmpl.txt失败"));
		return false;
	}
	//替换数据(external0,appendHtml0用作自己发信息使用,不改变)
	QFile fileWrite("Resources/MainWindow/MsgHtml/msgtmpl.js");
	bool isOpen = fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate);
	if (isOpen) {
		//更新空值
		QString strSourceInitNull = "var external = null;";
		//更新初始化
		QString strSourceInit = "external = channel.objects.external;";
		//更新newWebChannel
		QString strSourceNew =
			"new QWebChannel(qt.webChannelTransport,\
			function(channel) {\
			external = channel.objects.external;\
		}\
		); \
			";
		//更新追加recvHtml,脚本中有"",无法直接进行字符串赋值，采用读文件方式
		QString strSourceRecvHtml;
		QFile fileRecvHtml(":/Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly)) {
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		}
		else {
			QMessageBox::information(this, QString("提示"), QString("读取recvHtml.txt失败"));
			return false;
		}

		//保存替换后的脚本
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;
		for (int i = 0; i < employeesList.length(); ++i) {
			//编辑替换后的空值
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceInitNull += strInitNull;
			strReplaceInitNull += "\n";

			//编辑替换后的初始值
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceInit += strInit;
			strReplaceInit += "\n";

			//编辑替换后的newWebChannel
			QString strNew = strSourceNew;
			strNew.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceNew += strNew;
			strReplaceNew += "\n";

			//编辑替换后的recvHtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strRecvHtml.replace("recvHtml", QString("recvHtml_%1").arg(employeesList.at(i)));
			strReplaceRecvHtml += strRecvHtml;
			strRecvHtml += "\n";
		}
		strFile.replace(strSourceInitNull, strReplaceInitNull);
		strFile.replace(strSourceInit, strReplaceInit);
		strFile.replace(strSourceNew, strReplaceNew);
		strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);
		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();
		return true;
	}
	else {
		QMessageBox::information(this, QString("提示"), QString("写msgtmpl.js失败"));
		return false;
	}
}

//追加网页信息
void TalkWindowShell::handleReceiveMsg(int senderEmployeeID, int msgType, QString strMsg)
{
	QMsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);
	if (msgType == 1) {
		msgTextEdit.document()->toHtml();
	}
	else
	{
		const int emotionWidth = 3;
		int emotionNum = strMsg.length() / emotionWidth;
		for (int i = 0; i < emotionNum; ++i) {
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}
	QString html = msgTextEdit.document()->toHtml();
	//文本html如果没有字体则添加字体
	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();
			// 将html文件里的 %1，用字符串 text 替换
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else
		{
			// this，当前聊天部件，作为父窗体
			QMessageBox::information(this, QString::fromLocal8Bit("提示"),
				QString::fromLocal8Bit("文件 msgFont.txt 不存在！"));
			return;
		}

		// 判断转换后，有没有包含 fontHtml
		if (!html.contains(fontHtml))
		{
			html.replace(strMsg, fontHtml);
		}
	}
	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	talkWindow->ui.msgWidget->appendMsg(html, QString::number(senderEmployeeID));
}

//添加新的聊天窗口
void TalkWindowShell::addTalkWindow(TalkWindow * talkWindow, TalkWindowItem * talkWindowItem,const QString& uid/*GroupType grouptype*/)
{
	ui.rightStackedWidget->addWidget(talkWindow);
	connect(m_emotionWindow, SIGNAL(signalEmotionHide()),
		talkWindow, SLOT(onSetEmotionBtnStatus()));
	connect(talkWindow, SIGNAL(faceBtnClicked(bool)),
		this, SLOT(onEmotionBtnClicked(bool)));
	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
	m_talkwindowItemMap.insert(aItem, talkWindow);

	aItem->setSelected(true);

	//设置头像,判断是群聊还是单聊
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1")
		.arg(uid);
	sqlDepModel.setQuery(strQuery);
	int rows = sqlDepModel.rowCount();
	if (rows == 0)//单聊
	{
		strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(uid);
		sqlDepModel.setQuery(strQuery);
	}
	QModelIndex index;
	index = sqlDepModel.index(0, 0);//行。列
	QImage img;
	img.load(sqlDepModel.data(index).toString());
	//const QPixmap pix(":/Resources/MainWindow/girl.png");
	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));
	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);
	onTalkWindowItemClicked(aItem);

	//左侧的talkWindowItem关闭信号点击
	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem, talkWindow, aItem, this]() {
		m_talkwindowItemMap.remove(aItem);//映射移除
		talkWindow->close();//右边窗口要关闭
		ui.listWidget->takeItem(ui.listWidget->row(aItem));//左侧列表项移除
		delete talkWindowItem;//资源释放
		ui.rightStackedWidget->removeWidget(talkWindow);//资源释放
		if (ui.rightStackedWidget->count() < 1) {
			this->close();
		}
		});
}

void TalkWindowShell::setCurrentWidget(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

QMap<QListWidgetItem*, QWidget*> TalkWindowShell::getTalkWindowItemMap()const
{
	return m_talkwindowItemMap;
}

//当talkwindow的发送按钮点击后，MsgWebView发射一个信号，talkwindowshell接收后执行槽函数updateSendTcpMsg发送信息内容
//定义发送格式(消息格式，数据包格式)
//文本数据包格式:群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(或群QQ号) + 信息类型 + 数据长度 + 数据
// ，群聊标志是1则是群QQ号，0则是员工QQ号
//表情数据包格式:群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(或群QQ号) + 信息类型 + 表情个数 + images + 数据
//msgType 0表情信息，1文本信息，2文件信息
void TalkWindowShell::updateSendTcpMsg(QString& strData, int& msgType, QString fileName)
{
	//先获取当前的聊天窗口,这里使用强制转换
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	QString talkId = curTalkWindow->getTalkId();
	//开始组合要发的数据
	QString strGroupFlag;//群聊标志
	QString strSend;//用于发送的数据
	if (talkId.length() == 4)//群QQ号(2000，2001，2002...)
	{
		strGroupFlag = "1";
	}
	else
	{
		strGroupFlag = "0";
	}
	int nstrDataLength = strData.length();//数据长度
	int dataLength = QString::number(nstrDataLength).length();//数据长度
	//const int sourceDataLength = dataLength;//原始数据长度
	QString strDataLength;
	if (msgType == 1)//发送文本信息
	{
		//文本信息长度约定为5位,占位，5位说明发了5万个字符，如hello 5个字符，占位文本信息长度为1
		if (dataLength == 1) {
			//在前面补0
			strDataLength = "0000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 2)
		{
			strDataLength = "000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 3)
		{
			strDataLength = "00" + QString::number(nstrDataLength);
		}
		else if (dataLength == 4)
		{
			strDataLength = "0" + QString::number(nstrDataLength);
		}
		else if (dataLength == 5)
		{
			strDataLength = QString::number(nstrDataLength);
		}
		else {
			QMessageBox::information(this, QString("提示"), QString("不合理的数据长度"));
		}
	//文本数据包格式:群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(或群QQ号) + 信息类型(1) + 数据长度 + 数据
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "1" + strDataLength + strData;
	}
	else if (msgType == 0)//表情信息
	{
//表情数据包格式:群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(或群QQ号) + 信息类型(0) + 表情个数 + images + 数据	
		strSend = strGroupFlag + gLoginEmployeeID + talkId + 
			"0" + strData;
	}
	else if (msgType == 2)//文件信息
	{
//文件数据包格式:群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(或群QQ号) + 信息类型(2) 
//								+ 文件长度 + "bytes" + 文件名称 + "data_begin" + 文件内容
		QByteArray bt = strData.toUtf8();
		QString strLength = QString::number(bt.length());//数据长度
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "2" +
			strLength + "bytes" + fileName + "data_begin" + strData;			
	}
	//组装完毕后开始向tcp里面发数据
	QByteArray dataBt;
	dataBt.resize(strSend.length());//字节数组大小重置为strSend的长度
	dataBt = strSend.toUtf8();//把strSend赋值给dataBt,dataBt传到tcp里
	m_tcpClientSocket->write(dataBt);//socket写入数据
}

void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if (curTalkWindow) {
		curTalkWindow->addEmotionImage(emotionNum);
	}
}


/***********************************************************************************
数据包格式
文本数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(群QQ号) + 信息类型(1) + 数据长度 + 文本数据
表情数据包格式：群聊标志+发信息员工QQ号+收信息员工QQ号(群QQ号)+信息类型(0)+表情个数+images+表情名称
文件数据包格式：群聊标志+发信息员工QQ号+收信息员工QQ号(群QQ号)+信息类型(2)+文件字节数+bytes+文件名
								+ data_begin + 文件数据
群聊标志占1位，0单聊，1群聊
信息类型占1位，0表情信息，1文本信息，2文件信息
QQ号占5位，群QQ号占4位，数据长度占5位，表情名称占3位
群聊标志为1时，数据包中没有收信息员工QQ号，而是收信息群QQ号
群聊标志为0时，数据包中没有收信息群QQ号，而是收信息员工QQ号
************************************************************************************/
void TalkWindowShell::processPendingData()
{//解包，消息格式拆分
	//端口中有未处理的数据时
	while (m_udpReceiver->hasPendingDatagrams())
	{
		const static int groupFlagWidth = 1;//群聊标志位为1
		const static int groupWidth = 4;//群聊宽度4
		const static int employeeWidth = 5;//员工QQ号宽度
		const static int msgTypeWidth = 1;//信息类型宽度
		const static int msgLengthWidth = 5;//文本信息长度的宽度
		const static int pictureWidth = 3;//表情名称宽度
		//开始拿数据
		QByteArray btData;
		btData.resize(m_udpReceiver->pendingDatagramSize());
		m_udpReceiver->readDatagram(btData.data(), btData.size());//读取udp数据到btData中
		QString strData = btData.data();
		QString strWindowID;//聊天窗口ID，群聊则是群号，单聊则是QQ号
		QString strSendEmployeeID, strRecevieEmployeeID;//发送及接收端QQ号
		QString strMsg;//保存发送的数据
		int msgLen;//数据长度
		int msgType;//数据类型

		strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth);
		//自己发的信息不做处理
		if (strSendEmployeeID == gLoginEmployeeID) {
			return;
		}
		if (btData[0] == '1') {//群聊
			
			//群QQ号
			strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];
			if (cMsgType == '1')
			{//信息类型是文本信息
				msgType = 1;
				msgLen = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth, msgLengthWidth)
							.toInt();
				strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth + msgLengthWidth
					, msgLen);
			}
			else if (cMsgType == '0')
			{//信息类型是表情信息
				msgType = 0;
				int posImages = strData.indexOf("images");//返回images第一次出现的位置
				strMsg = strData.right(strData.length() - posImages - QString("images").length());
			}
			else if (cMsgType == '2')
			{//信息类型是文件信息
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int posData_begin = strData.indexOf("data_begin");
				//文件名称
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;
				//文件内容
				int dataLengthWidth;
				int posData = posData_begin + QString("data_begin").length();
				strMsg = strData.mid(posData);
				gfileData = strMsg;
				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1")
					.arg(employeeID));
				querySenderName.exec();
				if (querySenderName.first()) {
					sender = querySenderName.value(0).toString();
				}
				//接收文件的后续操作
				ReceiveFile* recvFile = new ReceiveFile(this);
				connect(recvFile, &ReceiveFile::refuseFile, [this]() {
					return;
					});
				QString msgLabel = QString("收到来自") + sender + QString("的文件,是否接收？");
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}
		else//单聊
		{
			strRecevieEmployeeID = strData.mid(groupFlagWidth + employeeWidth,employeeWidth);
			strWindowID = strSendEmployeeID;
			//不是发给我的信息不做处理
			if (strRecevieEmployeeID != gLoginEmployeeID) {
				return;
			}
			//获取信息类型0 10001 10002 1 00005 Hello
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
			if (cMsgType == '1')
			{//信息类型是文本信息
				msgType = 1;
				msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth, msgLengthWidth)
					.toInt();
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth + msgLengthWidth
					, msgLen);
			}
			else if (cMsgType == '0')
			{//信息类型是表情信息
				msgType = 0;
				int posImages = strData.indexOf("images");//返回images第一次出现的位置
				strMsg = strData.mid(posImages + QString("images").length());
			}
			else if (cMsgType == '2')
			{//信息类型是文件信息
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidth = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");
				//文件名称
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;
				//文件内容
				strMsg = strData.mid(posData_begin + data_beginWidth);
				gfileData = strMsg;
				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1")
					.arg(employeeID));
				querySenderName.exec();
				if (querySenderName.first()) {
					sender = querySenderName.value(0).toString();
				}
				ReceiveFile* recvFile = new ReceiveFile(this);
				connect(recvFile, &ReceiveFile::refuseFile, [this]() {
					return;
					});
				QString msgLabel = QString("收到来自") + sender + QString("的文件,是否接收？");
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}
		//将聊天窗口设为活动窗口
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget) {
			//聊天窗口存在
			this->setCurrentWidget(widget);
			//同步激活左侧聊天窗口
			QListWidgetItem* item = m_talkwindowItemMap.key(widget);
			item->setSelected(true);
		}
		else {//聊天窗口未打开
			return;
		}
		//文件信息另作处理
		if(msgType != 2)
		{
			int sendEmployeeID = strSendEmployeeID.toInt();
			handleReceiveMsg(sendEmployeeID, msgType, strMsg);//追加网页信息
		}
	}
}

void TalkWindowShell::onEmotionBtnClicked(bool) {
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));//将当前控件的相对位置转换为绝对位置
	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);
}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item) {
	QWidget* talkwindowWidget = m_talkwindowItemMap.find(item).value();
	ui.rightStackedWidget->setCurrentWidget(talkwindowWidget);
}
