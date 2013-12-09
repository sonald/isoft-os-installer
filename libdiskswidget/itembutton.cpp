#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QDir>

#include <sys/mount.h>
#include <unistd.h>

#include "directorytree.h"
#include "itembutton.h"

const int colFsType = 4;

ItemButton::ItemButton(QTreeWidgetItem *item, const QString &text, const QString &originPath)
	: QPushButton(text)
{
	parentItem = item;
	if (originPath.isEmpty())
		device = item->text(0);
	else
		device = originPath;
	connect(this, SIGNAL(clicked()), this, SLOT(showDir()));
}

void ItemButton::showDir()
{
	int pid = getpid();
	QString mnt = "/tmp/libdiskswidget" + QString::number(pid);
	QDir d;
	if (!d.exists(mnt)) {
		d.mkdir(mnt);
	}

	QString mntType;
	if (parentItem->text(colFsType).contains("fat", Qt::CaseInsensitive))
		mntType = "vfat";
	else
		mntType = parentItem->text(colFsType);
		
	QString command = "mount -n -t " + mntType + " " + device + " " + mnt;
	if (system(command.toLatin1())) {
		QMessageBox::warning(this, tr("sorry"), tr("You can't look over the partition what you select."));
		return ;
	}
	
	DirectoryTree *tree = new DirectoryTree(mnt, this);
	tree->setWindowTitle(device);
	tree->exec();

	system(("/bin/umount " + mnt).toLatin1());
	d.rmdir(mnt);
}
