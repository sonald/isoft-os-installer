#ifndef DIRECTORYTREE_H
#define DIRECTORYTREE_H

#include <QDialog>

class DirectoryTree : public QDialog
{
	Q_OBJECT
public:
	DirectoryTree(const QString &dir, QWidget *parent = 0);
};

#endif 
