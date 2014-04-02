#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QtDebug>

#include "addpartition.h"
#include "diskswidget.h"
#include "archinfo.h"

AddPartition::AddPartition(DisksWidget *parent) : QDialog(parent), _tree(parent)
{
	setupUi(this);
    this->retranslateUi(this);

    connect(chboxUseAll, SIGNAL(toggled(bool)), this, SLOT(toggleUseAll(bool)));
	connect(fsType, SIGNAL(currentIndexChanged(const QString &)), 
		this, SLOT(judgeMountEnable(const QString &)));
	connect(mntPoint, SIGNAL(currentIndexChanged(const QString &)), 
		this, SLOT(updateFsTypeList(const QString &)));

    if (!_tree->isEfiEnabled()) {
        //remove efi mount point
        int idx = this->mntPoint->findText(efiMountPoint);
        if (idx != -1) {
            this->mntPoint->removeItem(idx);
        }

        // in legacy mode, bios_grub partition is needed for grub to embed when 
        // disk has gpt table.
        
        if (_tree->maybeGPT(_tree->currentDevPath())) {
            fsType->addItem("bios_grub");
        }
    }

    ArchInfo ai;
    if (ai.arch() == ArchInfo::Loongson) {
        this->mntPoint->addItem("/boot");
    }

    fsType->setCurrentIndex(2);
}

void AddPartition::toggleUseAll(bool val)
{
    fixedSize->setEnabled(!val);
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

    if (!chboxUseAll->isChecked()) {
        int newVal = _tree->clampNewPartitionSize(fixedSize->value());
        if (newVal != fixedSize->value()) {
            chboxUseAll->setChecked(true);
        }
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
	} else if (mnt == efiMountPoint) {
        if (fsType->currentText() != "fat32") {
            QMessageBox::warning(this, tr("Warning"), 
                    tr("The 'efi' partition needs to be formatted as fat32."));
            return;
        }
    }
			
	QDialog::accept();
}

void AddPartition::updateFsTypeList(const QString &mntpoint)
{
    if (mntpoint == efiMountPoint) {
        int idx = fsType->findText("fat32");
        if (idx != -1) {
            fsType->setCurrentIndex(idx);
        }
    }
}

void AddPartition::judgeMountEnable(const QString &text)
{
	qDebug() << __FUNCTION__;
	if (text == "linux-swap" || text == "bios_grub") {
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
