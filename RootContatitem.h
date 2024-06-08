#pragma once

#include <QLabel>
#include <QPropertyAnimation>
//继承QLabel,这个类用于实现CCMainWindow窗口的根项右边箭头展开收缩有动画
class RootContatitem  : public QLabel
{
	Q_OBJECT
		//箭头角度
	Q_PROPERTY(int rotation READ rotation WRITE setRotation);//角度的动态属性
public:
	RootContatitem(bool hasArrow = true ,QWidget *parent = nullptr);
	~RootContatitem();
public:
	void setText(const QString& title);//设置文本

	void setExpanded(bool expand);//是否展开
private:
	int rotation();//返回角度

	void setRotation(int rotation);//设置角度

protected:
	void paintEvent(QPaintEvent* event);//重写一下绘图事件

private:
	QPropertyAnimation* m_animation;//动画，QPropertyAnimation是动画类

	QString m_titleText;//标题文本

	int m_rotation;//箭头角度

	bool m_hasArrow;//是否有箭头
};
