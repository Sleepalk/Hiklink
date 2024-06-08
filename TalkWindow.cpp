#include "TalkWindow.h"
#include "TalkWindowShell.h"
#include<qwidget.h>
#include<qtreewidget.h>
#include"RootContatitem.h"
#include"ContactItem.h"
#include"CommonUtils.h"
#include"WindowManager.h"
#include<qtooltip.h>
#include<qmessagebox.h>
#include<qfile.h>
#include<qsqlquerymodel.h>
#include<qsqlquery.h>
#include"SendFile.h"
TalkWindow::TalkWindow(QWidget* parent,const QString& uid/*, GroupType groupType*/)
	: QWidget(parent)
	,m_talkId(uid)
	//,m_groupType(groupType)
{
	ui.setupUi(this);
	WindowManager::getInstance()->addWindowName(m_talkId,this);
	initGroupTalkStatus();//在初始化控件之前，先初始化群聊状态
	initControl();
	this->setAttribute(Qt::WA_DeleteOnClose);
	connect(ui.faceBtn, SIGNAL(QPushButton::clicked(bool)),this,SLOT(onFaceBtnClicked(bool)));
}

TalkWindow::~TalkWindow()
{
	WindowManager::getInstance()->deleteWindowName(m_talkId);
}

//添加表情图片
void TalkWindow::addEmotionImage(int emotionNum)
{
	ui.textEdit->setFocus();//设置焦点
	ui.textEdit->addEmotionUrl(emotionNum);

}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);
}

QString TalkWindow::getTalkId()
{
	return m_talkId;
}

void TalkWindow::initControl()
{
	QList<int> rightWidgetSize;
	rightWidgetSize << 600 << 138;
	ui.bodySplitter->setSizes(rightWidgetSize);

	ui.textEdit->setFontPointSize(10);
	ui.textEdit->setFocus();

	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
	connect(ui.fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onfileopenBtnClicked(bool)));
	if (m_isGroupTalk) {
		//如果是群聊
		initTalkWindow();//初始化群聊
	}
	else {
		//单聊
		initPtoPTalk();//初始化单聊
	}
	//switch (m_groupType)
	//{
	//case COMPANY:
	//{
	//	initCompanyTalk();//初始化公司群
	//	break;
	//}
	//case PERSONELGROUP:
	//{
	//	initPersonelTalk();//
	//	break;
	//}
	//case DEVELOPMENTGROUP:
	//{
	//	initDevelopTalk();
	//	break;
	//}
	//case MARKETGROUP:
	//{
	//	initMarketTalk();
	//	break;
	//}
	//default:
	//{
	//	initPtoPTalk();//初始化单聊
	//	break;
	//}
	//}
}

void TalkWindow::initGroupTalkStatus()
{
	QSqlQueryModel sqlDepModel;
	QString strSql = QString("SELECT * FROM tab_department WHERE departmentID = %1").arg(m_talkId);
	sqlDepModel.setQuery(strSql);
	int rows = sqlDepModel.rowCount();
	if (rows == 0)//单独聊天
	{
		m_isGroupTalk = false;
	}
	else {
		m_isGroupTalk = true;
	}
}

void TalkWindow::initTalkWindow()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);//设置孩子指示器策略
	pRootItem->setData(0, Qt::UserRole, 0);//设置根项的数据为0,用于区分根项，子项
	RootContatitem* pItemName = new RootContatitem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);
	//当前聊天的群组名
	QString strGroupName;
	QSqlQuery queryGroupName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1")
		.arg(m_talkId));
	queryGroupName.exec();
	if (queryGroupName.next()) {
		strGroupName = queryGroupName.value(0).toString();
	}
	//
	QSqlQueryModel queryEmployeeModel;
	if (getCompDepID() == m_talkId.toInt()) {//如果是公司群
		queryEmployeeModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
	}
	else {
		queryEmployeeModel.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status = 1 AND departmentID = %1")
			.arg(m_talkId));
	}
	int nEmployeeNum = queryEmployeeModel.rowCount();//员工数量
	QString qsGroupName = QString("%1 %2/%3").arg(strGroupName).arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);
	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);//根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//展开
	pRootItem->setExpanded(true);
	for (int i = 0; i < nEmployeeNum; ++i) {
		QModelIndex modelIndex = queryEmployeeModel.index(i, 0);
		int employeeID = queryEmployeeModel.data(modelIndex).toInt();

		//添加子节点
		addPeopInfo(pRootItem,employeeID);
	}
}

/*
void TalkWindow::initCompanyTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);//设置孩子指示器策略
	pRootItem->setData(0, Qt::UserRole, 0);//设置根项的数据为0,用于区分根项，子项
	RootContatitem* pItemName = new RootContatitem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);
	int nEmployeeNum = 50;
	QString qsGroupName = QString("公司群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);
	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);//根项
	ui.treeWidget->setItemWidget(pRootItem,0,pItemName);

	//展开
	pRootItem->setExpanded(true);
	for (int i = 0; i < nEmployeeNum; ++i) {
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initPersonelTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);//设置孩子指示器策略
	pRootItem->setData(0, Qt::UserRole, 0);//设置根项的数据为0,用于区分根项，子项
	RootContatitem* pItemName = new RootContatitem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);
	int nEmployeeNum = 5;
	QString qsGroupName = QString("人事部 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);
	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);//根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//展开
	pRootItem->setExpanded(true);
	for (int i = 0; i < nEmployeeNum; ++i) {
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initDevelopTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);//设置孩子指示器策略
	pRootItem->setData(0, Qt::UserRole, 0);//设置根项的数据为0,用于区分根项，子项
	RootContatitem* pItemName = new RootContatitem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);
	int nEmployeeNum = 32;
	QString qsGroupName = QString("研发部 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);
	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);//根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//展开
	pRootItem->setExpanded(true);
	for (int i = 0; i < nEmployeeNum; ++i) {
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initMarketTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);//设置孩子指示器策略
	pRootItem->setData(0, Qt::UserRole, 0);//设置根项的数据为0,用于区分根项，子项
	RootContatitem* pItemName = new RootContatitem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);
	int nEmployeeNum = 8;
	QString qsGroupName = QString("市场部 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);
	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);//根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//展开
	pRootItem->setExpanded(true);
	for (int i = 0; i < nEmployeeNum; ++i) {
		addPeopInfo(pRootItem);
	}
}
*/
void TalkWindow::initPtoPTalk()
{
	QPixmap pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png");

	ui.widget->setFixedSize(pixSkin.size());			// 设置尺寸

	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(pixSkin);
	skinLabel->setFixedSize(ui.widget->size());
}

int TalkWindow::getCompDepID()
{//获取公司ID
	QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg("公司群"));
	queryDepID.exec();
	queryDepID.first();
	int companyID = queryDepID.value(0).toInt();
	return companyID;
}

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem,int employeeID)
{//为根项添加联系人
	QTreeWidgetItem* pChild = new QTreeWidgetItem();
	
	//添加子节点数据
	pChild->setData(0, Qt::UserRole, 1);
	pChild->setData(0, Qt::UserRole + 1, employeeID);
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	//获取名，签名，头像
	QString strName, strSign, strPicturePath;
	QSqlQueryModel queryInfoModel;
	queryInfoModel.setQuery(QString("SELECT employee_name,employee_sign,picture FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	QModelIndex nameIndex, signIndex, pictureIndex;
	nameIndex = queryInfoModel.index(0, 0);//0行0列
	signIndex = queryInfoModel.index(0, 1);
	pictureIndex = queryInfoModel.index(0, 2);

	strName = queryInfoModel.data(nameIndex).toString();
	strSign = queryInfoModel.data(signIndex).toString();
	strPicturePath = queryInfoModel.data(pictureIndex).toString();
	QPixmap pixl;
	pixl.load(":/Resources/MainWindow/head_mask.png");
	QImage imageHead;
	imageHead.load(strPicturePath);

	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(imageHead), pixl, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strName);
	pContactItem->setSignName(strSign);

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	QString str = pContactItem->getUserName();
	m_groupPeopleMap.insert(pChild, str);

}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (bIsChild) {//如果双击的是子项
		QString strPeopleName = m_groupPeopleMap.value(item);
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
	}
}

void TalkWindow::onSendBtnClicked(bool clicked) {
	if (ui.textEdit->toPlainText().isEmpty()) {
		QToolTip::showText(this->mapToGlobal(QPoint(630,660)), "发送信息不能为空",
			this,QRect(0,0,120,100),2000);
		return;
	}
	QString html = ui.textEdit->document()->toHtml();

	// 文本html，如果没有字体，则添加字体
	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QString text = ui.textEdit->toPlainText();
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();
			// 将html文件里的 %1，用字符串 text 替换
			fontHtml.replace("%1", text);
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
			html.replace(text,fontHtml);
		}
	}

	ui.textEdit->clear();
	ui.textEdit->deleAllEmotionImage();
	ui.msgWidget->appendMsg(html);//收信息窗口添加信息

}


void TalkWindow::onfileopenBtnClicked(bool clicked) {
	SendFile* sendFile = new SendFile();
	sendFile->show();
}