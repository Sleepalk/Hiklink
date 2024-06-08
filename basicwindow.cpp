#include "basicwindow.h"
#include"CommonUtils.h"
#include "titlebar.h"
#include<qfile.h>
#include<qstyleoption.h>
#include<qstyle.h>
#include<qpainter.h>
#include<qapplication.h>
#include"NotifyManager.h"
//#include<QDesktopWidget>
#include<qdesktopservices.h>
#include<QScreen>
#include<qsqlquery.h>
extern QString gLoginEmployeeID;
//用于其他窗口的基类
BasicWindow::BasicWindow(QWidget *parent)
	: QDialog(parent)
{
	m_colorBackGround = CommonUtils::getDeFaultSkinColor();
	this->setWindowFlags(Qt::FramelessWindowHint);//设置窗口风格为无边框
	this->setAttribute(Qt::WA_TranslucentBackground, false);//设置窗口部件透明化
	connect(NotifyManager::getInstance(), SIGNAL(signalSkinChanged(const QColor&)), this,
		SLOT(onSignalSkinChanged(const QColor&)));
}

BasicWindow::~BasicWindow()
{}

//窗口皮肤改变
void BasicWindow::onSignalSkinChanged(const QColor& color) {
	m_colorBackGround = color;
	//窗口颜色改变之后要对样式表重新加载
	loadStyleSheet(m_styleName);
}

void BasicWindow::setTitleBarTitle(const QString& title, const QString& icon = "") {
	_titleBar->setTitleIcon(icon);
	_titleBar->setTitleContent(title);
}

//初始化标题栏
void BasicWindow::initTitleBar(ButtonType buttontype) {
	_titleBar = new TitleBar(this);
	_titleBar->setButtonType(buttontype);
	_titleBar->move(0, 0);

	connect(_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	connect(_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
	connect(_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
	connect(_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));
}

//加载样式表
void BasicWindow::loadStyleSheet(const QString& sheetName) {
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		this->setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());
		//获取用户当前的皮肤RGB值
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		qsstyleSheet += QString("QWidget[titleskin=true]\
												{background-color:rgb(%1,%2,%3);\
												border-top-left-radius:4px;}\
												QWidget[bottomskin=true]\
												{border-top:1px solid rgba(%1,%2,%3,100);\
												background-color:rgba(%1,%2,%3,50);\
												border-bottom-left-radius:4px;\
												border-bottom-right-radius:4px;}")
												.arg(r).arg(g).arg(b);
		//设置样式表
		this->setStyleSheet(qsstyleSheet);
	}
	file.close();
}

//初始化背景
void BasicWindow::initBackGroundColor() {
	//该方法通过使用默认样式绘制原始形状来初始化窗口的背景颜色
	QStyleOption opt;
	opt.initFrom(this);
	QPainter painter(this);
	
	style()->drawPrimitive(QStyle::PE_Widget,&opt,&painter,this);//使用应用程序提供的样式来绘制小部件的背景
}

//重绘事件,子类化部件时，需要重写绘图事件设置背景颜色
void BasicWindow::paintEvent(QPaintEvent* event) {
	initBackGroundColor();//初始化背景
	QDialog::paintEvent(event);
}

//获取圆头像
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap& mask, QSize masksize) {
	//mask是一个空的圆头像,mask是可选的遮罩大小
	if (masksize == QSize(0, 0)) {
		masksize = mask.size();
	}
	else {
		mask = mask.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		//调用 scaled 函数以保持比例缩放遮罩
	}
	//保存转换后的头像
	QImage resultImage(masksize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	//设置图片叠加模式
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(masksize,Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();

	return QPixmap::fromImage(resultImage);
}

void BasicWindow::onShowClose(bool) {
	close();
}

void BasicWindow::onShowMin(bool) {
	showMinimized();
}

void BasicWindow::onShowHide(bool) {
	hide();
}

void BasicWindow::onShowNormal(bool) {
	this->show();
	this->activateWindow();//设为活动窗口
}


//整体窗口退出
void BasicWindow::onShowQuit(bool) {
	//更新登录状态为离线
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 1 WHERE employeeID = %1")
		.arg(gLoginEmployeeID);
	QSqlQuery sqlStatus(strSqlStatus);
	sqlStatus.exec();
	QApplication::quit();
}

void BasicWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		m_mousePressed = true;
		//pos()事件发生时相对于窗口左上角(0,0)的偏移
		m_mousePoint = event->globalPos() - pos();
		event->accept();
	}
}

void BasicWindow::mouseReleaseEvent(QMouseEvent* event) {
	m_mousePressed = false;
	event->accept();
}

void BasicWindow::mouseMoveEvent(QMouseEvent* event) {
	if (m_mousePressed && (event->buttons() && Qt::LeftButton)) {
		this->move(event->globalPos() - m_mousePoint);
		event->accept();
	}
}

void BasicWindow::onButtonCloseClicked() {
	this->close();
}

void BasicWindow::onButtonMaxClicked() {
	_titleBar->saveRestoreInfo(pos(), QSize(width(), height()));
	QScreen* screen = QGuiApplication::primaryScreen();
	QRect desktopRect = screen->availableVirtualGeometry();
	QRect factRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3,
		desktopRect.width() + 6, desktopRect.height() + 6);
	this->setGeometry(factRect);
}

void BasicWindow::onButtonMinClicked() {
	if (Qt::Tool == (windowFlags() & Qt::Tool)) {
		this->hide();
	}
	else {
		this->showMinimized();
	}
}

void BasicWindow::onButtonRestoreClicked() {
	QPoint windowPos;
	QSize windowSize;
	_titleBar->getRestoreInfo(windowPos, windowSize);
	this->setGeometry(QRect(windowPos, windowSize));
}