#pragma once
#include<qpixmap.h>
#include<qsize.h>
#include<qproxystyle.h>
//常用操作类

class CustomProxyStyle :public QProxyStyle {
	//自定义类，改变默认的部件风格
public:
	CustomProxyStyle(QObject* parent) {
		setParent(parent);
	}
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = 0)const {
		if (PE_FrameFocusRect == element) {
			//去掉window中部件默认的边框或虚线框，部件获取焦点时直接返回
			return;
		}
		else {
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};

class CommonUtils
{
public:
	CommonUtils();
public:
	static QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize masksize = QSize(0, 0));
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);//加载样式表
	static void setDefaultSkinColor(const QColor& color);
	static QColor getDeFaultSkinColor();
};

