#include "SkinWindow.h"
#include "basicwindow.h"
#include <qcolor.h>
#include <qlist.h>
#include "QClickLabel.h"
#include "NotifyManager.h"
#include<qpalette.h>
SkinWindow::SkinWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	this->loadStyleSheet("SkinWindow");
	setAttribute(Qt::WA_DeleteOnClose);//关闭窗口，资源立即释放
	initControl();
}

SkinWindow::~SkinWindow()
{}

void SkinWindow::initControl()
{	
	//定义一个颜色链表
	QList<QColor> colorList = {
		QColor(22,154,218),QColor(40,138,221),QColor(49,166,107),QColor(218,67,68),
		QColor(177,99,158),QColor(107,81,92),QColor(89,92,160),QColor(21,156,199),
		QColor(79,169,172),QColor(155,183,154),QColor(128,77,77),QColor(240,188,189)
	};
	for (int row = 0; row < 3; ++row) {
		for (int column = 0; column < 4; ++column) {
			QClickLabel* label = new QClickLabel(this);
			label->setCursor(Qt::PointingHandCursor);

			//c++11新特性lambda表达式，label被点击后发射点击信号，通知其他窗口改变皮肤
			connect(label, &QClickLabel::clicked, [row, column, colorList]() {
				//获取实例，通知其他窗口改变皮肤
				NotifyManager::getInstance()->notifyOtherWindowChangeSkin(colorList.at(row * 4 + column));
				});
			label->setFixedSize(84, 84);

			//设置调色板
			QPalette palette;
			palette.setColor(QPalette::Window, colorList.at(row * 4 + column));
			label->setAutoFillBackground(true);//设置背景自动填充
			label->setPalette(palette);
			ui.gridLayout_2->addWidget(label,row,column);
		}

		connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowMin(bool)));
		connect(ui.sysclose, SIGNAL(clicked()), this, SLOT(onShowClose()));
	}
}


void SkinWindow::onShowClose() {
	this->close();
}
