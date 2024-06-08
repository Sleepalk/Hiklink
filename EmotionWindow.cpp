#include "EmotionWindow.h"
#include "CommonUtils.h"
#include "EmotionLabelItem.h"
#include <qstyleoption.h>
#include<qpainter.h>
const int emotionColumn = 14;
const int emotionRow = 12;
EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);//窗体风格无边框，子窗口
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setAttribute(Qt::WA_DeleteOnClose);//回收
	initControl();
}

EmotionWindow::~EmotionWindow()
{}

void EmotionWindow::initControl()
{
	CommonUtils::loadStyleSheet(this, "EmotionWindow");

	for (int row = 0; row < emotionRow; ++row) {
		for (int column = 0; column < emotionColumn; ++column) {
			EmotionLabelItem* emotionlabel = new EmotionLabelItem(this);
			emotionlabel->setEmotionName(row * emotionColumn + column);
			connect(emotionlabel, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::addEmotion);
			ui.gridLayout->addWidget(emotionlabel, row, column);
		}
	}
}

void EmotionWindow::addEmotion(int emotionNum) {
	hide();
	emit signalEmotionWindowHide();
	emit signalEmotionItemClicked(emotionNum);
}

void EmotionWindow::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.initFrom(this);
	QPainter painter(this);
	this->style()->drawPrimitive(QStyle::PE_Widget,&opt,&painter,this);
	__super::paintEvent(event);
}
