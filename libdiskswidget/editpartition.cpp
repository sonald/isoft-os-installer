#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

#include "editpartition.h"
#include "diskswidget.h"

EditPartition::EditPartition(QTreeWidgetItem *currentItem, DisksWidget *parent) 
	: QDialog(parent), 
	  m_parent(parent), 
	  m_cur(currentItem) 
{
	setupUi(this);
	fsTypeLabel->setText(currentItem->text(colFs));
	sizeLabel->setText(currentItem->text(colSize));
	
	if (m_cur->text(colDev) == "free" || m_cur->text(colFs) == "Unknown"
		|| m_cur->text(colFs) == "linux-swap" )
		mntPointComboBox->setEnabled(false);

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
}

void EditPartition::accept()
{
	QString mnt = mntPointComboBox->currentText();
	if (!mnt.isEmpty() && m_parent->existMntPoint(mnt, m_cur)) {
		QMessageBox::warning(this, tr("Warning"), QString(tr("There already has a %1 mount point.")).arg(mnt));
		return ;
	}

	if (mnt == "/") {
		QString total = sizeLabel->text();
		if (!DisksWidget::isEnoughForSlash(total)) {
			QMessageBox::warning(this, tr("Warning"), QString(tr("The '/' partition at least need 4GB disk capacity.")));
			return ;
		}

		if ( !( (formatButton->isChecked() && formatComboBox->currentText() == "ext3") 
				|| (formatButton->isChecked() && formatComboBox->currentText() == "ext2")
				|| (unchangeButton->isChecked() && fsTypeLabel->text() == "ext3")))
		{
			QMessageBox::warning(this, tr("Warning"), QString(tr("The '/' partition needs to be formatted to \"ext3\" or \"ext2\".")));
			return ;
		}
			
	}

	QDialog::accept();
}

void EditPartition::checkSwap(const QString &text)
{
	if (text == "linux-swap") {
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
		if (m_cur->text(colDev) == "free" || m_cur->text(colFs) == "Unknown"
			|| m_cur->text(colFs) == "linux-swap" )
		{
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
		if (formatComboBox->currentText() == "linux-swap") {
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
		formatComboBox->setCurrentIndex(formatComboBox->findText("ext3"));
	} else {
		unchangeButton->setEnabled(true);
//		unchangeButton->setChecked(true);
	}
}
