#ifndef ITEMBUTTON_H
#define ITEMBUTTON_H

#include <QPushButton>

class QString;
class QTreeWidgetItem;

class ItemButton : public QPushButton
{
	Q_OBJECT
public:
	ItemButton(QTreeWidgetItem *item, const QString &text, const QString &originPath = "");

public slots:
	void showDir();
	
private:
	QString	device;
	QTreeWidgetItem *parentItem;
};

#endif
