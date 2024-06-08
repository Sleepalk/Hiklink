#pragma once

#include <QWebEngineView>
#include<QDomNode>
class MsgHtmlObj :public QObject {
	Q_OBJECT
	//发来的信息
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)
	//发出的信息
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)
public:
	MsgHtmlObj(QObject* parent,QString msgLPicPath = "");//发信息来的人的头像路径
signals:
	void signalMsgHtml(const QString& html);
private:
	void initHtmlTmpl();//初始化聊天网页
	QString getMsgtmplHtml(const QString& code);
private:
	QString m_msgLPicPath;//左侧头像路径
	QString m_msgLHtmlTmpl;//发来的信息
	QString m_msgRHtmlTmpl;//发出的信息
};

class MsgWebPage :public QWebEnginePage {
	Q_OBJECT
public:
	MsgWebPage(QObject* parent = nullptr):QWebEnginePage(parent){}
protected:
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame);
};

//QWebEngineView视图用于显示网页内容
class MsgWebView  : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget *parent);
	~MsgWebView();
	void appendMsg(const QString& html,QString strObj = "0");
private:
	QList<QStringList> parseHtml(const QString& html);//解析html,返回的是字符串链表

	//Qt中所有Dom节点(属性，说明，文本)都可以使用QDomNode表示
	QList<QStringList> parseDocNode(const QDomNode& node);//解析节点
signals:
	void signalSendMsg(QString& strData, int& msgTypt, QString sFile = "");
private:
	MsgHtmlObj* m_msgHtmlObj;
	QWebChannel* m_channel;
};
