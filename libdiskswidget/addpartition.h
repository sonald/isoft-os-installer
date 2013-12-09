#ifndef ADDPARTITION_H
#define ADDPARTITION_H

#include <QDialog>
#include <QTreeWidget>

#include "ui_add.h"

class QTreeWidgetItem;
class DisksWidget;
 
class AddPartition : public QDialog, public Ui::New
{
	Q_OBJECT
public:
	AddPartition(DisksWidget *parent = 0);

public slots:
	void accept();

private:
	bool 	isValidMountPath(const QString &mountPath);
	void	cleanSlash(QString &path);

private:
	DisksWidget *_tree;

private slots:
	void judgeMountEnable(const QString &text);
};

#endif
