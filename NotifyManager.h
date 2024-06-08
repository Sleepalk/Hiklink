#pragma once

#include <QObject>

//通知管理器类
class NotifyManager  : public QObject
{
	Q_OBJECT

public:
	NotifyManager();
	~NotifyManager();
signals:
	void signalSkinChanged(const QColor& color);
public:
	static NotifyManager* getInstance();//获取实例
	void notifyOtherWindowChangeSkin(const QColor& color);
private:
	static NotifyManager* instance;//实例
};
