#pragma once

#include <QLabel>
#include<QMouseEvent>

/*重写标签QLabel类，点击app图标更改皮肤时弹出skinwindow窗口，skinwindow窗口中有12个QLabel颜色标签，
点击颜色标签进行界面颜色更改，所以点击标签时得获取鼠标点击事件，发出信号*/
class QClickLabel  : public QLabel
{
	Q_OBJECT

public:
	QClickLabel(QWidget *parent);
	~QClickLabel();
protected:
	void mousePressEvent(QMouseEvent* event);
signals:
	void clicked();
};