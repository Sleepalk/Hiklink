#include "MsgWebView.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include"TalkWindowShell.h"
#include"WindowManager.h"
#include<qsqlquerymodel.h>
#include<qsqlquery.h>
extern QString gstrLoginHeadPath;

MsgHtmlObj::MsgHtmlObj(QObject* parent,QString msgLPicPath) : QObject(parent)
{
	m_msgLPicPath = msgLPicPath;
	initHtmlTmpl();
}

void MsgHtmlObj::initHtmlTmpl()
{
	// 初始化左侧
	m_msgLHtmlTmpl = getMsgtmplHtml("msgleftTmpl");
	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);

	m_msgRHtmlTmpl = getMsgtmplHtml("msgrightTmpl");
	m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
}

QString MsgHtmlObj::getMsgtmplHtml(const QString& code)
{
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");

	file.open(QFile::ReadOnly);
	QString strData;

	if (file.isOpen())
	{
		// 全部一次性读取
		strData = QLatin1String(file.readAll());
	}
	else
	{
		// information（父窗体指针，标题，提示内容）;
		// 因为当前写代码的对象，是从 QObject 派生来的，不是从某个部件派生来的
		// 因此要写 空
		QMessageBox::information(nullptr, "Tips", "Failed to init html!");
		return QString("");			// 返回空
	}

	file.close();

	return strData;
}


bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	// 仅接受 qrc:/*.html
	// 判断 URL 类型，是否为 qrc类型
	if (url.scheme() == QString("qrc"))
	{
		return true;
	}
	else
	{
		return false;
	}

}


// QWebEngineView 视图，用于 显示网页内容
MsgWebView::MsgWebView(QWidget* parent)
	: QWebEngineView(parent)
	,m_channel(new QWebChannel(this))
{
	// 构造一个网页，并 设置为 当前的网页
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	m_msgHtmlObj = new MsgHtmlObj(this);

	// 注册
	m_channel->registerObject("external0", m_msgHtmlObj);


	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);

	//当前正构建的聊天窗口的id
	QString strTalkId = WindowManager::getInstance()->getCreatingTalkId();
	QSqlQueryModel queryEmployeeModel;
	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroupTalk = false;
	//获取公司群ID
	queryEmployeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg("公司群"));
	QModelIndex companyIndex = queryEmployeeModel.index(0, 0);
	QString strCompanyID = queryEmployeeModel.data(companyIndex).toString();
	if (strTalkId == strCompanyID) {//公司群聊
		isGroupTalk = true;
		queryEmployeeModel.setQuery("SELECT employeeID,picture FROM tab_employees WHERE status = 1");
	}
	else
	{
		if (strTalkId.length() == 4) {//其他群聊
			isGroupTalk = true;
			queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1 \
					AND departmentID = %1").arg(strTalkId));
		}
		else {//单独聊天
			queryEmployeeModel.setQuery(QString("SELECT picture FROM tab_employees WHERE status = 1 \
					AND employeeID = %1").arg(strTalkId));
			QModelIndex index = queryEmployeeModel.index(0, 0);
			strPicturePath = queryEmployeeModel.data(index).toString();
			strExternal = "external_" + strTalkId;
			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}
	}
	if (isGroupTalk)
	{
		QModelIndex employeeModelIndex, pictureModelIndex;
		int rows = queryEmployeeModel.rowCount();
		for (int i = 0; i < rows; i++) {
			employeeModelIndex = queryEmployeeModel.index(i, 0);
			pictureModelIndex = queryEmployeeModel.index(i, 1);
			strEmployeeID = queryEmployeeModel.data(employeeModelIndex).toString();
			strPicturePath = queryEmployeeModel.data(pictureModelIndex).toString();
			strExternal = "external_" + strEmployeeID;
			
			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}
	}
	// 设置当前网页，网络通道
	this->page()->setWebChannel(m_channel);
	// 加载网页，进行初始化
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{
}

void MsgWebView::appendMsg(const QString& html, QString strObj)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList> msgList = parseHtml(html);		// 解析 html

	int msgType = 1;//信息类型：0是表情，1是文本，2是文件
	QString strData;
	bool isImageMsg = false;//图片信息
	int imageNum = 0;//表情数字
	for (int i = 0; i < msgList.size(); i++)
	{
		// 如果 msgList当前循环的，链表的 第一个元素
		// 是 img 文件
		if (msgList.at(i).at(0) == "img")
		{
			// 获得 图片路径
			// 这里写1，0是img，1是图片的真实内容
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;		// 用于加载图片
			isImageMsg = true;
			//获取表情名称的位置
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();

			QString strEmotionName = imagePath.mid(pos);//截取到111.png
			strEmotionName.replace(".png", "");//把.png用空替代,如strEmotionName = 111
			//根据表情名称的长度进行设置表请数据，不足3位则补足3位
			int emotionNameL = strEmotionName.length();
			if (emotionNameL == 1) {
				strData = strData + "00" + strEmotionName;
			}
			else if (emotionNameL == 2) {
				strData = strData + "0" + strEmotionName;
			}
			else if (emotionNameL == 3) {
				strData = strData + strEmotionName;
			}
			msgType = 0;//发送类型为表情
			imageNum++;
			// 判断最左边的 3个字符，是否为 qrc
			if (imagePath.left(3) == "qrc")		// 假设路径为 qrc:/MainWindow/xxx
			{
				// mid() 方法，从中间位置开始截取
				// 从第三个字符开始截取，就是 " : " 冒号开始
				// 去掉表情路径中 qrc
				pixmap.load(imagePath.mid(3));
			}
			else
			{	// 不是 qrc的，就直接加载
				pixmap.load(imagePath);
			}
			// 表情图片html格式，文本组合（路径，宽度，高度）
			QString imgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\"/>")
				.arg(imagePath).arg(pixmap.width()).arg(pixmap.height());

			// 存到 字符串中
			qsMsg += imgPath;
		}
		else if (msgList.at(i).at(0) == "text")
		{
			// 这里写1，0是text，1是文本的真实内容
			qsMsg += msgList.at(i).at(1);
			strData = qsMsg;
		}
	}

	// 插入到 Json 对象中，是键值对，
	// "MSG"是键，qsMsg是值
	// 体现在，资源文件中，以 msgleftTmpl.html 为例
	// 里面的内容中的 【MSG】 <div class="msg">{{MSG}}<span class="trigon">
	msgObj.insert("MSG", qsMsg);

	// 一切的目的，都是为了 转成QString 字符串类型
	// 再转换成 Json 文档，并且 要转成 UTF-8 的文档
	// QJsonDocument::Compact，紧凑的意思
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	if (strObj == "0") {
		//发信息
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));
		if (isImageMsg)
		{
			strData = QString::number(imageNum) + "images" + strData;
		}
		emit signalSendMsg(strData, msgType);
	}
	else {
		//收信息
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	}
}

QList<QStringList> MsgWebView::parseHtml(const QString& html)
{
	// 因为，传进来的是 html 文件，将它转换成 QT节点文件
	QDomDocument doc;
	doc.setContent(html);					// 转换

	// 想要解析的节点，就是 html里面的 body
	// 想拿到body，需要先获取元素
	// 节点元素
	const QDomElement& root = doc.documentElement();
	// 获取 第一个元素，node 是节点类型，还需要对节点进行 解析
	const QDomNode& node = root.firstChildElement("body");

	return parseDocNode(node);
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	// 需要 最终解析出来的，是 字符串链表
	QList<QStringList> attribute;
	// list 保存，返回所有子节点
	const QDomNodeList& list = node.childNodes();

	// list.count()，链表的长度
	for (int i = 0; i < list.count(); i++)
	{
		// 挨个获取，当前链表中的 节点
		const QDomNode& node = list.at(i);

		// 再对节点，进行解析，判断 是否为 元素
		if (node.isElement())
		{
			// toElement() 方法，直接转换成 元素
			const QDomElement& element = node.toElement();

			// 判断 元素名 是否为 图片
			if (element.tagName() == "img")
			{
				// 获取图片的值
				QStringList attributeList;
				// 拿到图片的路径，表情，人头像
				attributeList << "img" << element.attribute("src");
				attribute << attributeList;
			}

			if (element.tagName() == "span")
			{
				QStringList attributeList;
				attributeList << "text" << element.text();
				attribute << attributeList;
			}

			// 如果 这个节点 ，还有 子节点
			if (node.hasChildNodes())
			{
				// 再 继续解析节点，解析之后 再将结果 存到 attribute
				attribute << parseDocNode(node);
			}
		}
	}

	return attribute;
}


