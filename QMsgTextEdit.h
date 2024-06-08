#pragma once

#include <QTextEdit>
#include"EmotionWindow.h"
#include"EmotionLabelItem.h"

//这个类用于提升编辑框textedit
/*
		因为，插入的图片是动图，所以帧改变的时候，
		文本编辑器内容的位置，也要相应的改变。
*/
class QMsgTextEdit  : public QTextEdit
{
	Q_OBJECT
private slots:
	void onEmotionImageFrameChange(int frame);//动画改变时响应的槽函数
public:
	void addEmotionUrl(int emotionNum);//添加表情资源
	void deleAllEmotionImage();//删除所有的图片资源，点击发送后textedit要清空

public:
	QMsgTextEdit(QWidget *parent = nullptr);
	~QMsgTextEdit();
private:
	QList<QString> m_listEmotionUrl;
	QMap<QMovie*, QString> m_emotionMap;
};
