#include <QTreeView>
#include <QDirModel>
#include <QPushButton>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "directorytree.h"

DirectoryTree::DirectoryTree(const QString &dir, QWidget *parent) : QDialog(parent)
{
	QTreeView *view = new QTreeView(this);
	QDirModel *model = new QDirModel;
	view->setModel(model);
	view->setRootIndex(model->index(dir));
	view->resizeColumnToContents(0);
	setWindowTitle(dir);

	QPushButton *ok = new QPushButton(tr("&OK"));
	QSpacerItem *item = new QSpacerItem(40, 20, QSizePolicy::Expanding);
	QHBoxLayout *bottom = new QHBoxLayout;
	bottom->addItem(item);
	bottom->addWidget(ok);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(view);
	layout->addLayout(bottom);
	setLayout(layout);
	setMinimumSize(600, 400);
//	resize(sizeHint());
	ok->setFocus();

	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
}
