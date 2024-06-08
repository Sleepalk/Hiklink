#include "RootContatitem.h"
#include<qpainter.h>
RootContatitem::RootContatitem(bool hasArrow,QWidget *parent)
	: QLabel(parent)
	,m_rotation(0)
	,m_hasArrow(hasArrow)//初始化
{
	this->setFixedHeight(32);//设置固定高度为32
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);//水平方向扩展，垂直方向固定


	//属性动画初始化，"rotation"：表示要进行动画的属性名称
	m_animation = new QPropertyAnimation(this,"rotation");
	m_animation->setDuration(50);//设置单次动画时间，这里设置为50毫秒
	m_animation->setEasingCurve(QEasingCurve::InQuad);//动画缓和曲线类型

}


RootContatitem::~RootContatitem()
{}

void RootContatitem::setText(const QString & title)
{
	m_titleText = title;
	update();//更新，重绘界面
}

//是否展开
void RootContatitem::setExpanded(bool expand)
{
	if (expand) {
		m_animation->setEndValue(90);//设置属性动画的结束值
	}
	else {
		m_animation->setEndValue(0);
	}
	m_animation->start();//动画启动
}

int RootContatitem::rotation()
{
	return m_rotation;
}

void RootContatitem::setRotation(int rotation)
{
	m_rotation = rotation;
	update();
}

//重绘事件
void RootContatitem::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::TextAntialiasing,true);//文本防锯齿

	QFont font;
	font.setPointSize(10);
	painter.setFont(font);
	painter.drawText(24, 0, this->width() - 24, this->height(), Qt::AlignLeft | Qt::AlignVCenter,m_titleText);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.save();

	if (m_hasArrow) {
		QPixmap pixmap;
		pixmap.load(":/Resources/MainWindow/arrow.png");

		QPixmap tmpPixmap(pixmap.size());
		tmpPixmap.fill(Qt::transparent);//填充透明

		QPainter p(&tmpPixmap);
		p.setRenderHint(QPainter::SmoothPixmapTransform, true);
		
		p.translate(pixmap.width() / 2, pixmap.height() / 2);//坐标系偏移(x方向偏移，y方向偏移)
		p.rotate(m_rotation);//旋转坐标系(顺时针)
		p.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2 , pixmap);


		painter.drawPixmap(6, (height() - pixmap.height()) / 2, tmpPixmap);
		painter.restore();//恢复画家设置
	}
	QLabel::paintEvent(event);
}
