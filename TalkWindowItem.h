#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget *parent = nullptr);
	~TalkWindowItem();
private:
	void initControl();
	void enterEvent(QEnterEvent* event);//鼠标进入事件
	void leaveEvent(QEvent* event);//鼠标离开事件
	void resizeEvent(QResizeEvent* event);
public:
	void setHeadPixmap(const QPixmap& pixmap);
	void setMsgLabelContent(const QString& msg);
	QString getMsgLabelText();
signals:
	void signalCloseClicked();
private:
	Ui::TalkWindowItemClass ui;
};
