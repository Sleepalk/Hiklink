#pragma once
#include"QClickLabel.h"
#include<qmovie.h>
/*这个类用来显示表情窗口的表情项，派生于QClickLabel类*/
class EmotionLabelItem  : public QClickLabel
{
	Q_OBJECT

public:
	EmotionLabelItem(QWidget *parent);
	~EmotionLabelItem();
	void setEmotionName(int emotionName);

private:
	void initControl();
signals:
	void emotionClicked(int emotionNum);
private:
	int m_emotionName;
	QMovie* m_apngMovie;
};
