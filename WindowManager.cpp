#include "WindowManager.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include<qsqlquerymodel.h>
#include<qsqlquery.h>
// 单例模式，创建全局静态对象
Q_GLOBAL_STATIC(WindowManager, theInstance)


WindowManager::WindowManager()
	: QObject()
	, m_talkwindowshell(nullptr)
{

}

WindowManager::~WindowManager()
{
}

QWidget* WindowManager::findWindowName(const QString& qsWindowName)
{
	// 根据传进来的窗口名称，来做判断
	// 映射中，如果包含 传进来的窗体
	if (m_windowMap.contains(qsWindowName))
	{
		// 如果包含，就返回 m_windowMap 的值
		return m_windowMap.value(qsWindowName);
	}

	return nullptr;			// 如果没有就返回空
}

void WindowManager::deleteWindowName(const QString& qsWindowName)
{
	// 从映射中，进行删除，就是关闭窗口
	// 根据窗口名称，进行删除
	m_windowMap.remove(qsWindowName);
}

void WindowManager::addWindowName(const QString& qsWindowName, QWidget* qWidget)
{
	// 在映射中，添加窗口名
	// 先判断，映射中有没有包含 传进来的窗口名
	// 没有的话，再进行添加
	if (!m_windowMap.contains(qsWindowName))
	{
		m_windowMap.insert(qsWindowName, qWidget);
	}
}

TalkWindowShell* WindowManager::getTalkWindowShell()
{
	return m_talkwindowshell;
}

WindowManager* WindowManager::getInstance()
{
	// 返回当前实例
	return theInstance();
}

void WindowManager::addNewTalkWindow(const QString& uid/*, GroupType groupType, const QString& strPeople*/)
{
	// 判断，先保证 父窗口已经创建出来了，才能添加
	// 如果是空的，就得先构造 父窗口
	if (m_talkwindowshell == nullptr)
	{	
		m_talkwindowshell = new TalkWindowShell;
		
		// 当 TalkWindowShell ，关闭，销毁的时候，
		// 对这个指针 m_talkwindowshell ，也进行一下销毁，
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* obj)
			{
				m_talkwindowshell = nullptr;
			});
	}

	// 判断，窗口的名字
	// 通过窗口唯一的身份证号 ID，来查找名字
	// 返回的是一个指针，指向的就是 这个窗体
	// 如果 widget 指针，是个空的，就说明映射中还没有ID，
	// 也就是还没有窗体，就要先创建 widget
	// 如果 widget 不是空的，就说明已经有ID了，已经存在 uid了
	QWidget* widget = findWindowName(uid);

	// 开始做判断
	if (!widget)
	{
		m_strCreatingTalkId = uid;
		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid/*, groupType*/);
		TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);
		m_strCreatingTalkId = "";
		//判断是群聊还是单聊
		QSqlQueryModel sqlDepModel;
		QString strSql = QString("SELECT department_name,sign FROM tab_department WHERE departmentID = %1")
						.arg(uid);
		sqlDepModel.setQuery(strSql);
		int rows = sqlDepModel.rowCount();
		QString strWindowName, strMsgLabel;
		if (rows == 0)//单聊
		{
			QString sql = QString("SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = %1")
				.arg(uid);
			sqlDepModel.setQuery(sql);
		}
		QModelIndex indexDepIndex, signIndex;
		indexDepIndex = sqlDepModel.index(0, 0);//0行0列
		signIndex = sqlDepModel.index(0, 1);//0行1列
		strWindowName = sqlDepModel.data(signIndex).toString();
		strMsgLabel = sqlDepModel.data(indexDepIndex).toString();
		talkwindow->setWindowName(strWindowName);
		talkwindowItem->setMsgLabelContent(strMsgLabel);
		// 添加聊天窗口
		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem,uid);
		
	}
	else
	{
		// 对左边进行设置，左侧聊天列表设为被选中
		// 通过这个 widget ，返回链表项
		// 调用 key，获得值
		QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);

		// 对右边进行设置 当前聊天窗口
		m_talkwindowshell->setCurrentWidget(widget);
	}

	m_talkwindowshell->show();
	m_talkwindowshell->activateWindow();			// 设置为活动窗体
}

QString WindowManager::getCreatingTalkId()
{
	return m_strCreatingTalkId;
}

