#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QtGui>
#include <QDebug>

#include "editpartition.h"
#include "diskswidget.h"
#include "archinfo.h"

EditPartition::EditPartition(QTreeWidgetItem *currentItem, DisksWidget *parent) 
	: QDialog(parent), 
	  m_parent(parent), 
	  m_cur(currentItem) 
{
	setupUi(this);
    this->retranslateUi(this);

	fsTypeLabel->setText(currentItem->text(colFs));
    formatComboBox->setCurrentIndex(2); // ext4
	sizeLabel->setText(currentItem->text(colSize));

    if (!allowMount()) {
		mntPointComboBox->setEnabled(false);
    }

	connect(formatButton, SIGNAL(toggled(bool )), formatComboBox, SLOT(setEnabled(bool )));
	connect(formatButton, SIGNAL(toggled(bool )), this, SLOT(setMountByFormatButton(bool )));
	connect(formatComboBox, SIGNAL(currentIndexChanged(const QString & )), this, SLOT(checkSwap(const QString & )));
	connect(unchangeButton, SIGNAL(toggled(bool )), this, SLOT(checkFree(bool )));
	connect(mntPointComboBox, SIGNAL(currentIndexChanged(const QString &)), SLOT(checkMountPoint(const QString &)));
      
	QString mount = currentItem->text(colMount);
	if (!mount.isEmpty()) {
		int index = mntPointComboBox->findText(mount);
		mntPointComboBox->setCurrentIndex(index);
	}

    if (!m_parent->isEfiEnabled()) {
        //remove efi mount point
        int idx = this->mntPointComboBox->findText("/boot/efi");
        if (idx != -1) {
            this->mntPointComboBox->removeItem(idx);
        }

        // in legacy mode, bios_grub partition is needed for grub to embed when 
        // disk has gpt table.
        if (m_parent->maybeGPT(m_parent->currentDevPath())) {
            formatComboBox->addItem("bios_grub");
        }
    }

    ArchInfo ai;
    if (ai.arch() == ArchInfo::Loongson) {
        this->mntPointComboBox->addItem("/boot");
    }
}

bool EditPartition::allowMount()
{
	return !(m_cur->text(colDev) == "free" || m_cur->text(colFs) == "Unknown"
		|| m_cur->text(colFs) == "linux-swap" || m_cur->text(colFs) == "bios_grub");
}

void EditPartition::accept()
{
	QString mnt = mntPointComboBox->currentText();
	if (!mnt.isEmpty() && m_parent->existMntPoint(mnt, m_cur)) {
		QMessageBox::warning(this, tr("Warning"), QString(tr("There already has a %1 mount point.")).arg(mnt));
		return ;
	}

	if (mnt == "/") {
        if (!((formatButton->isChecked() && formatComboBox->currentText().startsWith("ext")) 
                || (unchangeButton->isChecked() && fsTypeLabel->text().startsWith("ext")))) {
			QMessageBox::warning(this, tr("Warning"), QString(tr("The '/' partition needs to be formatted to one of ext filesystem.")));
			return ;
		}
			
	} else if (mnt == "/boot/efi") {
        if (formatButton->isChecked() && formatComboBox->currentText() != "fat32") {
			QMessageBox::warning(this, tr("Warning"), QString(tr("The '/' partition needs to be formatted to one of ext filesystem.")));
			return ;
		}
			
	}

	QDialog::accept();
}

void EditPartition::checkSwap(const QString &text)
{
	if (text == "linux-swap" || text == "bios_grub") {
		mntPointComboBox->setCurrentIndex(-1);
		mntPointComboBox->setEnabled(false);
	} else {
		mntPointComboBox->setEnabled(true);
	}
}

void EditPartition::checkFree(bool checked)
{
	qDebug() << __FUNCTION__;
	//k only Unchanged button check, disable mount combo
	if (checked) {
		if (!allowMount()) {
			mntPointComboBox->setEnabled(false);
			mntPointComboBox->setCurrentIndex(-1);
		} else {
			mntPointComboBox->setEnabled(true);
		}
	}
}

void EditPartition::setMountByFormatButton(bool checked)
{
	if (checked) {
		if (formatComboBox->currentText() == "linux-swap" || formatComboBox->currentText() == "bios_grub") {
			mntPointComboBox->setCurrentIndex(-1);
			mntPointComboBox->setEnabled(false);
		 } else
			mntPointComboBox->setEnabled(true);
	}
}

void EditPartition::checkMountPoint(const QString &mntPoint)
{
	if (mntPoint == "/") {
		unchangeButton->setEnabled(false);
		formatButton->setChecked(true);
		formatComboBox->setCurrentIndex(formatComboBox->findText("ext4"));
	} else {
		unchangeButton->setEnabled(true);
	}
}
