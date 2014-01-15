#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QtDebug>

#include "addpartition.h"
#include "diskswidget.h"

AddPartition::AddPartition(DisksWidget *parent) : QDialog(parent), _tree(parent)
{
	setupUi(this);
    this->retranslateUi(this);

	connect(userButton, SIGNAL(toggled(bool)), userSize, SLOT(setEnabled(bool)));
	connect(fsType, SIGNAL(currentIndexChanged(const QString &)), 
		this, SLOT(judgeMountEnable(const QString &)));

    if (!_tree->isEfiEnabled()) {
        //remove efi mount point
        int idx = this->mntPoint->findText("/boot/efi");
        if (idx != -1) {
            this->mntPoint->removeItem(idx);
        }
    }
}

void AddPartition::accept()
{
	if (!mntPoint->isEnabled()) {
		QDialog::accept();
		return ;
	}

	QString mnt = mntPoint->currentText();

	if (mnt.isEmpty()) {
		QDialog::accept();
		return ;
	}
		
	if (!isValidMountPath(mnt)) {
		QMessageBox::warning(this, tr("Warning"), tr("Please set a valid directory as mount point."));
		return ;
	}

	if (mnt.isEmpty()) {
		QMessageBox::warning(this, tr("Warning"), tr("Please select a mount point."));
		return ;
	}

	if (_tree->existMntPoint(mnt)) {
		QMessageBox::warning(this, tr("Warning"), QString(tr("There already has a %1 mount point.")).arg(mnt));
		return;
	}

	cleanSlash(mnt);
	mntPoint->setEditText(mnt);
	qDebug() << mnt;
		
	if (mnt == "/") {
        if (!fsType->currentText().startsWith("ext")) {
			QMessageBox::warning(this, tr("Warning"), 
                    tr("The '/' partition needs to be formatted to one of the ext filesystems."));
			return;
		}
	}
			
	QDialog::accept();
}

void AddPartition::judgeMountEnable(const QString &text)
{
	qDebug() << __FUNCTION__;
	if (text == "linux-swap") {
		mntPoint->setCurrentIndex(-1);
		mntPoint->setEnabled(false);
	} else {
		mntPoint->setEnabled(true);
	}
}

bool AddPartition::isValidMountPath(const QString &mountPath)
{
	if (mountPath.at(0) != '/')
		return false;

	QRegExp re("[^/\\w\\d\\_\\-]");
	QStringList list = mountPath.split(QRegExp("/"));

	QStringList::iterator it = list.begin();
	for (++it; it+1 != list.end(); ++it) {
		if ((*it).contains(re))
			return false;
	}
	
	if ( !(*it).isEmpty() && (*it).contains(re))
		return false;

	
	//k just for debug
	if (!re.isValid())
		qDebug() << "re is NOT valid";
	else
		qDebug() << "re is valid";

	qDebug() << mountPath;
	return true;
}

void AddPartition::cleanSlash(QString &path)
{
	QStringList list = path.split("/");
	QString str = "/";

	QStringList::iterator it = list.begin() + 1;
	for ( ; it+1 != list.end(); ++it) {
		if (!(*it).isEmpty())
			str += (*it) + "/"; 
	}

	if (!(*it).isEmpty())
		str += (*it);

	path = str;
}
