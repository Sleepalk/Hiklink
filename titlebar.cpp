#include "titlebar.h"
#include<QHBoxLayout>
#include<qpainter.h>
#include<qpainterpath.h>
#include<QMouseEvent>
#include<qfile.h>
//定义一些宏
#define BUTTON_HEIGHT 27		//按钮高度为27像素
#define	BUTTON_WIDTH 27			//按钮宽度为27像素
#define TITLE_HEIGHT 27			//标题栏高度27像素

TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent)
	,m_isPressed(false)
	,m_buttonType(MIN_MAX_BUTTON)
{
	initControl();//初始化控件
	initConnections();//初始化信号与槽的连接
	loadStyleSheet("Title");//加载样式表，样式表写在文件里面
}

TitleBar::~TitleBar()
{}

//初始化控件函数实现
void TitleBar::initControl() {
	m_pIcon = new QLabel(this);
	m_pTitleContent = new QLabel(this);
	m_pButtonMin = new QPushButton(this);
	m_pButtonRestore = new QPushButton(this);
	m_pButtonClose = new QPushButton(this);
	m_pButtonMax = new QPushButton(this);

	//设置按钮为固定大小尺寸,27*27像素
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

	//设置对象名,方便在代码中查找和操作对象
	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonClose->setObjectName("ButtonClose");

	//设置标题栏的布局
	QHBoxLayout* mylayout = new QHBoxLayout(this);
	mylayout->addWidget(m_pIcon);
	mylayout->addWidget(m_pTitleContent);
	mylayout->addWidget(m_pButtonMin);
	mylayout->addWidget(m_pButtonRestore);
	mylayout->addWidget(m_pButtonMax);
	mylayout->addWidget(m_pButtonClose);

	mylayout->setContentsMargins(5, 0, 0, 0);//设置布局的间隙
	mylayout->setSpacing(0);//设置布局里部件间的间隙

	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);//标题内容的尺寸策略
	//在水平方向上可以扩展以填充可用空间，但在垂直方向上保持固定的尺寸。

	setFixedHeight(TITLE_HEIGHT);//设置标题栏固定高度
	setWindowFlags(Qt::FramelessWindowHint);//设置窗体风格为无边框
}

//信号与槽的连接
void TitleBar::initConnections() {
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

//设置标题栏图标
void TitleBar::setTitleIcon(const QString& filePath) {
	QPixmap titleIcon(filePath);
	m_pIcon->setFixedSize(titleIcon.size());
	m_pIcon->setPixmap(titleIcon);
}

//设置标题栏内容
void TitleBar::setTitleContent(const QString& titleContent) {
	m_pTitleContent->setText(titleContent);
	m_TitleContent = titleContent;//对标题内容进行更新
}

//设置标题栏宽度
void TitleBar::setTitleWidth(int width) {
	this->setFixedWidth(width);
}

//设置标题栏按钮类型
void TitleBar::setButtonType(ButtonType buttontype) {
	m_buttonType = buttontype;//更新按钮类型

	switch (buttontype)
	{
	case MIN_BUTTON: {		//只有最小化和关闭按钮
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
	}
		break;
	case MIN_MAX_BUTTON: {	//最小化，最大化及关闭按钮
		m_pButtonRestore->setVisible(false);
	}
		break;
	case ONLY_CLOSEBUTTON: {	//只有关闭按钮
		m_pButtonMin->setVisible(false);
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
	}
		break;
	default:
		break;
	}
}

//保存窗口最大化前窗口的位置及大小
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size) {
	m_restorePos = point;
	m_restoreSize = size;
}

//获取窗口最大化前窗口的位置和大小
void TitleBar::getRestoreInfo(QPoint& point, QSize& size) {
	point = m_restorePos;
	size = m_restoreSize;
}

//绘制标题栏
void TitleBar::paintEvent(QPaintEvent* event) {
	//设置背景色
	QPainter painter(this);
	QPainterPath pathBack;//绘制路径
	pathBack.setFillRule(Qt::WindingFill);//设置填充规则
	pathBack.addRoundedRect(QRect(0, 0, width(), height()),3,3);//添加圆角矩形到绘图路径
	painter.setRenderHint(QPainter::SmoothPixmapTransform,true);//平滑

	//当窗口最大化或还原后，窗口长度改变，标题栏相应做出改变
	//parentWidget()返回父部件
	if (this->width() != parentWidget()->width()) {
		setFixedWidth(parentWidget()->width());
	}
	QWidget::paintEvent(event);
}

//鼠标事件,实现拖动标题栏达到移动窗口效果
void TitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
	//主要实现双击标题栏进行最大化最小化操作
	//只有存在最大化最小化按钮才有效
	if (m_buttonType == MIN_MAX_BUTTON) {
		if (m_pButtonMax->isVisible()) {
			this->onButtonMaxClicked();
		}
		else
			this->onButtonRestoreClicked();
	}
	//return QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::mousePressEvent(QMouseEvent* event) {
	if (m_buttonType == MIN_MAX_BUTTON) {
		//在窗口最大化时禁止拖动
		if (m_pButtonMax->isVisible()) {
			m_isPressed = true;
			m_startMovePos = event->globalPos();//globalPos()返回事件发生时鼠标所在的全局位置
		}
	}
	else {
		m_isPressed = true;
		m_startMovePos = event->globalPos();//globalPos()返回事件发生时鼠标所在的全局位置
	}
	return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
	if (m_isPressed) {
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPos = parentWidget()->pos();//返回部件位置
		m_startMovePos = event->globalPos();
		parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
	}
	return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event) {
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

//加载样式表
void TitleBar::loadStyleSheet(const QString& sheetName) {
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		QString styleSheet = this -> styleSheet();//获取当前样式表
		styleSheet += QLatin1String(file.readAll());
		setStyleSheet(styleSheet);
	}
	file.close();
}

//各种槽函数实现
void TitleBar::onButtonMinClicked() {
	emit signalButtonMinClicked();
}

void TitleBar::onButtonRestoreClicked() {
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);
	emit signalButtonRestoreClicked();
}

void TitleBar::onButtonMaxClicked() {
	m_pButtonRestore->setVisible(true);
	m_pButtonMax->setVisible(false);
	emit signalButtonMaxClicked();
}

void TitleBar::onButtonCloseClicked() {
	emit signalButtonCloseClicked();
}