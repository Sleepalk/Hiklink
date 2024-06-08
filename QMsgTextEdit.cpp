#include "QMsgTextEdit.h"
#include<qmovie.h>
#include<DocumentSource.h>
#include<qtextdocument.h>
#include<qurl.h>
void QMsgTextEdit::onEmotionImageFrameChange(int frame) {
	QMovie* movie = qobject_cast<QMovie*>(sender());
	document()->addResource(QTextDocument::ImageResource,
		QUrl(m_emotionMap.value(movie)), movie->currentPixmap());
}

void QMsgTextEdit::addEmotionUrl(int emotionNum)
{
	const QString& imageName = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(emotionNum);
	const QString& flagName = QString("%1").arg(imageName);
	insertHtml(QString("<img src='%1' />").arg(flagName));
	if (m_listEmotionUrl.contains(imageName)) {
		return;
	}
	else {
		m_listEmotionUrl.append(imageName);
	}
	QMovie* apngMovie = new QMovie(imageName, "apng", this);
	m_emotionMap.insert(apngMovie, flagName);
	connect(apngMovie,SIGNAL(frameChanged(int)),this,SLOT(onEmotionImageFrameChange(int)));//数据帧改变时发射的信号
	apngMovie->start();
	updateGeometry();
}

void QMsgTextEdit::deleAllEmotionImage()
{
	for (auto itor = m_emotionMap.constBegin(); itor != m_emotionMap.constEnd(); ++itor) {
		delete itor.key();
	}
	m_emotionMap.clear();
}

QMsgTextEdit::QMsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{}

QMsgTextEdit::~QMsgTextEdit()
{
	deleAllEmotionImage();
}
