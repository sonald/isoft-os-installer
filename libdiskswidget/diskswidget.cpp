/*	diskswidget.cpp -- a QTreeWidget present all disks and partitions, 
 *			   provide operation on them
 *
 * 	Copyright 1999, 2008 Neil Kang <kaikang@redflag-linux.com>
 * 	Copyright 2014 Sian Cao <siyuan.cao@i-soft.com.cn>
 *
 *   	This program is free software; you can redistribute it and/or modify
 *   	it under the terms of the GNU General Public License as
 *   	published by the Free Software Foundation; either version 2,
 *   	or (at your option) any later version.
 *
 *   	This program is distributed in the hope that it will be useful,
 *   	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   	GNU General Public License for more details
 *
 *   	You should have received a copy of the GNU Library General Public
 *   	License along with this program; if not, write to the
 *   	Free Software Foundation, Inc.,
 *   	51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QTreeWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QtDebug>
#include <QDir>
#include <QSpacerItem>
#include <QTranslator>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/mount.h>
#include <cassert>

#include <parted++/parted++.h>
#include <installengine.h>
#include <os_identify.h>
#include <QtCore>

#include "diskswidget.h"
#include "directorytree.h"
#include "itembutton.h"
#include "editpartition.h"
#include "addpartition.h"

QTranslator translator;
DisksWidget::DisksWidget(QWidget *parent, const Modes &mode, QString locale) 
	: QWidget(parent), m_mode(mode)
{
	if (locale.isEmpty())
		locale = QLocale::system().name();
    QLocale::setDefault(locale);

	translator.load("libdiskswidget_" + locale, AppPath + "/translations");
    QApplication::installTranslator(&translator);

	m_osIdent = new OSIdentify;

	m_tree = new QTreeWidget;
	m_devices = new PartedDevices;

	QStringList header;
	header << tr("Disk/Partition") << tr("Partition Type") << tr("Total Size")
		<< tr("Size Used") << tr("File System") << tr("OS Type") << tr("Mount Point") << tr("Format") << "" << ""
		<< "begin block" << "end block";
	m_tree->setHeaderLabels(header);
	m_tree->hideColumn(colPartType);
	m_tree->hideColumn(colBeginBlock);
	m_tree->hideColumn(colEndBlock);
	m_tree->setMinimumHeight(200);
//	m_tree->setUniformRowHeights(true);
	
	initTree();

	QHBoxLayout *bottom = new QHBoxLayout;
	m_add = new QPushButton(tr("Cre&ate"), this);
	m_add->setEnabled(false);

	m_edit = new QPushButton(tr("&Edit"), this);
	m_del = new QPushButton(tr("&Delete"), this);
	QPushButton *m_reset = new QPushButton(tr("&Reset"), this);
	QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);

	bottom->addItem(spacer);
	bottom->addWidget(m_reset);
	bottom->addWidget(m_edit);
	bottom->addWidget(m_add);
	bottom->addWidget(m_del);

	_layout = new QVBoxLayout;
	_layout->addWidget(m_tree);
	_layout->addLayout(bottom);
	setLayout(_layout);

	connect(m_add, SIGNAL(clicked()), this, SLOT(addPartition()));
	connect(m_edit, SIGNAL(clicked()), this, SLOT(editPartition()));
	connect(m_del, SIGNAL(clicked()), this, SLOT(delPartition()));
	connect(m_reset, SIGNAL(clicked()), this, SLOT(reset()));
	
	connect(m_tree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(sendPartInfo(QTreeWidgetItem *, QTreeWidgetItem *)));
	connect(m_tree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(setNEWState(QTreeWidgetItem *, QTreeWidgetItem *)));

	connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem *, int )),SLOT(create_fs_table(QTreeWidgetItem *, int )));
	connect(m_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )),SLOT(editPartition()));

	switch (mode) {
	case Simple:
		m_tree->hideColumn(colMount);
		m_tree->hideColumn(colFormat);
		m_add->hide();
		m_edit->hide();
		m_del->hide();
		m_reset->hide();
//		bottom->hide();
		break;

	case Advanced:
		break;

	case Linux:
		m_tree->clearSelection();

	case Windows:
		m_add->hide();
		m_edit->hide();
		m_del->hide();
		m_reset->hide();

		m_tree->setMinimumHeight(100);
		m_tree->hideColumn(colMount);
		m_tree->hideColumn(colFormat);

		int count = 0;
		while (count < m_tree->topLevelItemCount()) { 
			QTreeWidgetItem *item = m_tree->topLevelItem(count);
			if (item->childCount() == 0) {
				m_tree->takeTopLevelItem(count);
				continue;
			}
			++count;
		}

		break;
	}

//	setMinimumSize(800, 300);

	QList<QString> tmp;
	m_commandList.push_back(QPair<QString, QList<QString> >("start_partition_section", tmp));
}

void DisksWidget::initTree()
{	
	for (int i = 0; i < m_devices->count(); ++i) {
		Device *disk = m_devices->device(i);
		if (!disk)
			continue;

		QTreeWidgetItem *parent = new QTreeWidgetItem(m_tree);
		parent->setText(0, disk->path());
		m_tree->expandItem(parent);

		if (disk->is_read_only())
			qDebug() << "read only";

		PartitionTable *table = disk->parttable();
        Q_ASSERT(table != NULL);
        qDebug() << "------" << table->type_name();
		PartitionList *partList = table->partlist();
		m_partitionListMap.insert(disk->path(), partList);

		int count = partList->count();
		qDebug() << "partition numbers is " << count;
		for (int j = 0; j < count; ++j) {
			Partition *partition = partList->part_index(j);
			if (partition == NULL)
				continue;

			QString partPath = partition->path();
			if (m_mode == Linux) {
				if (!isLinux(partPath))
					continue;
			} else if (m_mode == Windows) {
				if (!isWindows(partPath))
					continue;
			}

			if (partPath.contains("-1"))
				partPath = "free";

			if (parent->text(1) == "extended" && partition->isPrimary())
				parent = parent->parent();
			QTreeWidgetItem *child = new QTreeWidgetItem(parent);
			child->setText(colDev, partPath);
			child->setText(colPartType, partition->type_name());
			child->setText(colFs, partition->fs_type_name());
			if (child->text(colFs) == "linux-swap")
				child->setText(colMount, "swap");

			child->setText(colSize, partition->length_str());

			QString mntType = child->text(colFs);
			if (mntType.contains("fat"))
				mntType = "vfat";
			
			QString used;
			if ( partPath != "free" && mntType != "Unknown" && !mntType.isEmpty()) 
			{
				if (calcSize(partPath, mntType, used)) 
					child->setText(colUsed, used);
			}

			child->setText(colOs, osOnDevice(partPath));

			if (child->text(1) != "extended" && child->text(1) != "free" 
				&& child->text(colFs) != "linux-swap" && child->text(colFs) != "Unknown") 
			{
				ItemButton *detail = new ItemButton(child, tr("detail"));
				m_tree->setItemWidget(child, colDetail, detail);
			}

			child->setText(colBeginBlock, QString::number(partition->start()));
			child->setText(colEndBlock, QString::number(partition->end()));

			m_tree->expandItem(child);
			if (child->text(colPartType) == "extended")
				parent = child;
			else if (child->text(colPartType) != "free"){

			//kk except extended partition itself??
			PartitionInfo partInfo;
			partInfo.disk = disk->path();
//			partInfo.index = j;
			int index = itemIndex(child);
			if (index == -1) {
				qWarning() << "get index error";
				return ;
			}

			partInfo.index = index;

			partInfo.devName = partPath;
			partInfo.begBlock = QString::number(partition->start());
			partInfo.endBlock = QString::number(partition->end());
			partInfo.format = false;
			if (child->text(colFs) == "linux-swap")
				partInfo.mntPoint = "swap";
			m_partInfoList.push_back(partInfo);

			//k record original partition info
			OriginalPartsChanges change;
			change.disk = disk->path();
			change.devPath = partPath;
			change.begBlock = QString::number(partition->start());
			change.endBlock = QString::number(partition->end());
			change.original = true;
			m_partsChangeList.push_back(change);
			}
		} //k for j
	}

	qDebug() << "m_partInfoList size is " << m_partInfoList.size();
	qDebug() << __FUNCTION__ << __LINE__;
	m_tree->update();
	m_tree->setCurrentItem(m_tree->topLevelItem(0));
	m_tree->resizeColumnToContents(colDev);
	m_tree->resizeColumnToContents(colSize);
	m_tree->resizeColumnToContents(colUsed);
//	m_tree->resizeColumnToContents(colOs);
}//k initTree()

void DisksWidget::reset()
{
	m_tree->clear();
	m_partInfoList.clear();
	m_partsChangeList.clear();
	m_commandList.clear();
	m_partitionListMap.clear();

	//k for not delete twice
	delete m_devices;
	m_devices = new PartedDevices;
	initTree();
}

//kk only permit to delete partition
void DisksWidget::delPartition()
{
	qDebug() << __PRETTY_FUNCTION__;
	QTreeWidgetItem *current = m_tree->currentItem();
	if (!current)
		return ;

	if (current->text(1).isEmpty()) {
		QMessageBox::warning(this, tr("Warning"), tr("You aren't permitted to delete a disk"));
		return;
	}

	if (current->text(1) == "free") {
		QMessageBox::warning(this, tr("Warning"), 
				tr("You aren't permitted to delete a free partition"));
		return;
	}

	int count = current->childCount(); 
	if (current->text(1) == "extended"  && 
		(count > 1 || ( count == 1 && current->child(0)->text(colDev) != "free"))) 
	{
		QMessageBox::warning(this, tr("Warning"), 
				tr("There are logical partition(s) on this extended partition."));
		return ;
	} 

	QString disk = belongedDisk(current);
			
	qDebug() << __FUNCTION__ << __LINE__;
	for (int i = 0; i < m_devices->count(); ++i) {
		Device *dev = m_devices->device(i);
		if (!dev)
			continue;

		if (disk == dev->path()) {
			PartitionList *partList = m_partitionListMap.value(dev->path());
			if (!partList)
				continue;
				
			Partition *partition = 0;
			int j = 0;
			for ( ; j < partList->count(); ++j) {
				Partition *tmp = partList->part_index(j);
				if (QString::number(tmp->start()) == current->text(colBeginBlock)
					&& QString::number(tmp->end()) == current->text(colEndBlock))
				{
					partition = tmp;
					break;
				}
			}

			if (!partition) {
				qWarning() << "Can't find the partition you are deleting, error.";
				return ;
			}
				
			int index = j;

			QList<PartitionInfo>::iterator partIt = m_partInfoList.begin();
			for ( ; partIt != m_partInfoList.end(); ++partIt) {
				if (partIt->disk == disk
					&& QString::number(partition->start()) == partIt->begBlock
					&& QString::number(partition->end()) == partIt->endBlock)
				{
					m_partInfoList.erase(partIt);
					break;
				}
			}

			//k if exists original partition, modify it in m_partsChangeList
			QList<OriginalPartsChanges>::iterator it = m_partsChangeList.begin();
			for ( ; it != m_partsChangeList.end(); ++it) {
				if (it->disk == disk
					&& QString::number(partition->start()) == it->begBlock 
					&& QString::number(partition->end()) == it->endBlock) 
				{
						if (it->original == true) {
							it->original = false;
							it->info = tr("deleted");
						}
						break;
				}
			}

			//k record command list
			QList<QString> tmp;
			tmp << current->text(colDev);
			qDebug() << tmp;
			m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_rmpart", tmp));
			
			partList->delete_index(index);
			break;
		}
	}

	rebuildTree();
	qDebug() << __FUNCTION__ << __LINE__;
}//k delPartition()

void DisksWidget::rebuildTree()
{
	m_tree->clear();
	for (int i = 0; i < m_devices->count(); ++i) {
		QTreeWidgetItem *parent = new QTreeWidgetItem(m_tree);

		Device *dev = m_devices->device(i);
		if (!dev)
			continue;

		QString disk = dev->path();
		qDebug() << disk;

		parent->setText(colDev, disk);
		m_tree->expandItem(parent);

		PartitionList *partList = m_partitionListMap.value(dev->path());

		int count = partList->count();
		for (int j = 0; j < count; ++j) {
			Partition *partition = partList->part_index(j);
			if (partition == NULL)
				continue;

			//k if primary partition beyond extended, reset parent
			if (parent->text(1) == "extended" && partition->isPrimary())
				parent = parent->parent();
				
			QTreeWidgetItem *child = new QTreeWidgetItem(parent);

			QString partPath = partition->path();
			qDebug() << partPath << partition->type_name();

			if (partPath.contains("-1"))
				partPath = "free";
			child->setText(colDev, partPath);
			child->setText(colPartType, partition->type_name());
			child->setText(colFs, partition->fs_type_name());         
			child->setText(colSize, partition->length_str());

			child->setText(colBeginBlock, QString::number(partition->start()));
			child->setText(colEndBlock, QString::number(partition->end()));

			//k if original partition, give USED& DETAIL button
			//k search in m_partsChangeList
			QList<OriginalPartsChanges>::iterator itc = m_partsChangeList.begin();
			bool orig = false;
			//k we are operating a partition list in memory, may be NOT coodinate with real partitions
			QString realPartPath;
			for ( ; itc != m_partsChangeList.end(); ++itc) {
				if (itc->disk == disk 
					&& itc->begBlock == child->text(colBeginBlock)
					&& itc->endBlock == child->text(colEndBlock))
				{
					orig = itc->original;
					realPartPath = itc->devPath;
					break;
				}
			}

			if (orig && child->text(colFs) != "linux-swap") {
				//k show OS
				child->setText(colOs, osOnDevice(realPartPath));

				//k show disk used
				QString mntType = child->text(colFs);
				if (mntType.contains("fat"))
					mntType = "vfat";
				QString used;
				if ( mntType != "Unknown" && !mntType.isEmpty() 
					&& calcSize(realPartPath, mntType, used)) 
				{
					child->setText(colUsed, used);
				}

				//k show detail button
				if ((child->text(colPartType) != "extended" && child->text(colPartType) != "free"
                            && child->text(colFs) != "linux-swap") && child->text(colFs) != "Unknown")  
				{
					ItemButton *detail = new ItemButton(child, tr("detail"), realPartPath);
					m_tree->setItemWidget(child, colDetail, detail);
				}
			} 

			//kk get the info from partInfoList
			QList<PartitionInfo>::iterator it = m_partInfoList.begin();
			for ( ; it != m_partInfoList.end(); ++it) {
				if (it->disk != disk)
					continue;

				if (it->begBlock == child->text(colBeginBlock) &&
					it->endBlock == child->text(colEndBlock) &&
					child->text(colPartType) != "extended")
				{
					if (child->text(colDev) != "free") {
						child->setText(colMount, it->mntPoint); 
						if (it->format)
							child->setText(colFormat, checkMark);
					}
					break;
				}
			}//k for it

			m_tree->expandItem(child);
			if (child->text(1) == "extended")
				parent = child;
		}
	}//k for j
//	m_tree->setCurrentItem(m_tree->topLevelItem(0));
	m_tree->resizeColumnToContents(colDev);
	m_tree->resizeColumnToContents(colSize);
	m_tree->resizeColumnToContents(colUsed);
} //kk rebuildTree()


void DisksWidget::editPartition()
{
	if (m_mode == Windows)
		return;

	QTreeWidgetItem *current = m_tree->currentItem();
	if (!current)
		return;

	if (current->text(1).isEmpty()) {
		QMessageBox::warning(this, tr("Warning"), tr("Can not edit a disk"));
		return;
	}

	if (current->text(1) == "extended") {
		QMessageBox::warning(this, tr("Warning"), tr("Can not edit extended partition"));
		return;
	}

	qDebug() << current->text(colFs);
	EditPartition *edit = new EditPartition(current, this);
	if (QDialog::Rejected == edit->exec())
		return ;

	//k accepted
	//k but as another mount point
	bool unChanged = edit->unchangeButton->isChecked();
	if (unChanged && edit->mntPointComboBox->currentText().isEmpty())
		return ;

	QString disk = belongedDisk(current);
    bool isGPT = maybeGPT(disk);

	//k only two option, one is format is checked, the other is unchanged
	QString format = edit->formatComboBox->currentText();
	if (format == "vfat")
		format = "fat32";

	PartitionList *partlist = m_partitionListMap.value(disk);
	int index = itemIndex(current);
	if (index == -1) {
		qWarning() << "get partition " << current->text(colDev) << " index error";
		return ;
	}

	QList<QString> tmp;
	//k if free patition, new... XXX
	//k XXX it maybe that re-calculate the index because of little partition
	if (current->text(colDev) == "free") {
		int count = primaryCount(disk);
		if (isPrimary(current)) {
			bool exist = existExtended(disk);
			if (!isGPT && count >= 4) {
				QMessageBox::warning(this, tr("Warning"), tr("You can't create primary partition any more."));
				return ;
			}
			
            if (isGPT || count < 3) {
				//k count < 3 so create primary whether exists extended
				partlist->add_by_whole(index, "primary", format.toLatin1());
				tmp.clear();
				tmp << disk << QString::number(index) << "primary" << format.toLatin1();
				m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));
			} else if (count == 3 && !exist) {
				//k create extended
				int no = partlist->add_by_whole(index, "extended", 0);
				index = partlist->find_index_of(no);

				//k create logical
				++index;
				partlist->add_by_whole(index, "logical", format.toLatin1());

				tmp << disk << QString::number(index - 1) << "extended" << "";
				m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));
				tmp.clear();
				tmp << disk << QString::number(index) << "logical" << format.toLatin1();
				m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));
			} 
            
		} else { 
			//k is logic partition
			partlist->add_by_whole(index, "logical", format.toLatin1());
			tmp.clear();
			tmp << disk << QString::number(index) << "logical" << format.toLatin1();
			m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));
		}
		QString tmpMount = edit->mntPointComboBox->currentText();
		if (edit->formatComboBox->currentText() == "linux-swap")
			tmpMount = "swap";
		addPartInfo(index, current, tmpMount);
	} else {
		// else not free part
		if (!unChanged) {
			partlist->set_part_fstype(index, format.toLatin1());
			tmp << partlist->part_index(index)->path() << format;
			m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkfs", tmp));
		}

		QList<PartitionInfo>::iterator it = m_partInfoList.begin();
		for ( ; it != m_partInfoList.end(); ++it) {
			//k what if more than one free partition?
			if (it->disk == disk && it->begBlock == current->text(colBeginBlock) 
				&& it->endBlock == current->text(colEndBlock)) 
			{
				it->mntPoint = edit->mntPointComboBox->currentText();
				if (edit->formatButton->isChecked()) {
					it->format = true;
					if (edit->formatComboBox->currentText() == "linux-swap")
						it->mntPoint = "swap";
				}
				break;
			}
		}
	}

	//k if format, record in m_partsChangeList
	if (edit->formatButton->isChecked()) {
		qDebug() << __LINE__ << "format button checked";
		QList<OriginalPartsChanges>::iterator it = m_partsChangeList.begin();
		for ( ; it != m_partsChangeList.end(); ++it) {
			if (it->disk == belongedDisk(current)
				&& it->begBlock == current->text(colBeginBlock)
				&& it->endBlock == current->text(colEndBlock))
			{
				it->original = false;
				it->info = tr("formatted");
			}
		}
	}

	m_tree->clear();
	rebuildTree();

}//k editPartition()

void DisksWidget::addPartition()
{
	QTreeWidgetItem *current = m_tree->currentItem();
    QString disk = belongedDisk(current);
    bool isGPT = maybeGPT(disk);
    if (!isGPT) {
        if (isPrimary(current)) {
            int cnt = primaryCount(disk); 
            if ( cnt >= 4) {
                QMessageBox::warning(this, tr("Warning"), tr("No more Primary partition can be created."));
                return ;
            }
        }

        if (partitionCount(disk) >= 15) {
            QMessageBox::warning(this, tr("Warning"), tr("You can not create partitions more than 15."));
            return ;
        }
    }

	if (current->text(colPartType).isEmpty()) {
		QMessageBox::warning(this, tr("Warning"), tr("You should not do this operation with a disk."));
		return ;
	}

	if (current->text(0) != "free") {
		QMessageBox::warning(this, tr("Warning"), tr("Please select a free partition to operate."));
		return ;
	}

	AddPartition *add = new AddPartition(this);
	if (add->exec() == QDialog::Rejected)
		return;
//	else too much more indentation

	//k get the parameters
	QString mntPoint = add->mntPoint->currentText();
	QString fsType = add->fsType->currentText();
	if (fsType == "linux-swap")
		mntPoint = "swap";
	qDebug() << fsType;
	if (fsType == "vfat")
		fsType = "fat32";

	QString length;
	if (add->fixedButton->isChecked())
		length = QString::number(add->fixedSize->value());
	else if (add->userButton->isChecked())
		length = QString::number(add->userSize->value());
	else
		length = "-1";

	qDebug() << length;
	//k XXX createPrimary() also record into commandlist
	if (isPrimary(current)) {
		int count = primaryCount(disk);
		if (!isGPT && count >= 4) {
			QMessageBox::warning(this, tr("Warning"), tr("Can not create a main partition any more."));
			return;
        }

		if (count < 3 || isGPT) { //kk create new primary
			qDebug() << length << fsType;
			int index = createPrimary(current, fsType, length, "primary");
			addPartInfo(index, current, mntPoint);

		} else { //k count = 3
			if (existExtended(disk) || (add->forcePrimary->isChecked())) {
				int index = createPrimary(current, fsType, length, "primary");
				addPartInfo(index, current, mntPoint);
			} else { 
				//k create extended
				int index = createPrimary(current, 0, "-1", "extended");
				PartitionList *list = m_partitionListMap.value(disk);

				//k create logical
				++index;
				qDebug() << index << __LINE__;
				int no;
				if (length == "-1") {
					no = list->add_by_whole(index, "logical", fsType.toLatin1());

					QList<QString> tmp;
					tmp << disk << QString::number(index) << "logical" << fsType;
					m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));
				} else {
					no = list->add_by_length(index, "logical", fsType.toLatin1(), length.toLatin1(), "MB");
//					getchar();

					QList<QString> tmp;
					tmp << disk << QString::number(index) << "logical" << fsType << length;
					m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_length", tmp));
				}

				//k delete the empty info first
				int index2 = list->find_index_of(no);
				addPartInfo(index2, current, mntPoint); //XXX format
			}
		}
	} else { //k logical partition
		PartitionList *list = m_partitionListMap.value(disk);
		int index = 0;
		Partition *partition = 0;
		for ( ; index < list->count(); ++index) {
			Partition *tmp = list->part_index(index);
			qDebug() << "tmp partition's path " << tmp->path();
			qDebug() << "part type " << current->text(colPartType);
			
			if (QString::number(tmp->start()) == current->text(colBeginBlock)
				&& QString::number(tmp->end()) == current->text(colEndBlock)
				&& tmp->type_name() != QString("extended"))
			{
				partition = tmp;
				break;
			}
		}
		if (!partition) {
			qWarning() << "Can't find index to create logical";
			return ;
		}

		qDebug() << "index is " << index;
		qDebug() << "parititon path name " << partition->path();
		int no = 0;
		if (length == "-1") {
			no = list->add_by_whole(index, "logical", fsType.toLatin1());
			
			QList<QString> tmp;
			tmp << belongedDisk(current) << QString::number(index) << "logical" << fsType;
			m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));
		} else {
			no = list->add_by_length(index, "logical", fsType.toLatin1(), length.toLatin1(), "MB");

			QList<QString> tmp;
			tmp << belongedDisk(current) << QString::number(index) << "logical" << fsType << length;
			m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_length", tmp));
		}

		int in = list->find_index_of(no);
		addPartInfo(in, current, mntPoint);
	}

	m_tree->clear();
	rebuildTree();
}//k addPartition

//k only for get disk used
//k XXX wohuang, I have said will fill /etc/fstab
bool DisksWidget::calcSize(const QString &dev, const QString &mntType, QString &used)
{
	//k Maybe after check /etc/fstab, I have to check /etc/mtab too
	int index = dev.lastIndexOf('/');
	QString basename = dev.right(dev.size() - (index + 1));
	qDebug() << basename;

	char temp[] = "/tmp/diskswidget.XXXXXX";
	char *dir = mkdtemp(temp);
	if (dir == NULL) 
		return false;

	QString mntPath = dir;
	QString cmd = "/bin/mount -t " + mntType + " " + dev + " " + mntPath;
	int ret = system(cmd.toLatin1());
	if (ret)
		return false;
	
	struct statfs buf;
	if (statfs(mntPath.toLatin1(), &buf) == -1) {
		return false;
	}

	qDebug() << dev << "reach here" << __LINE__;
//	long lsize = (buf.f_blocks * (buf.f_bsize / 1024 )) / 1024;
//	long lused = ((buf.f_blocks - buf.f_bfree) * (buf.f_bsize / 1024) ) / 1024;
	double size = (buf.f_blocks - buf.f_bfree) * (buf.f_bsize / 1024.0);
	qDebug() << "size " << size;
	int i = 0;
	for (i = 0; i < 3; ++i) {
		if (size > 1024)
			size /= 1024;
		else 
			break;
	}

	char bufsize[80];
	sprintf(bufsize, "%.1f", size);
	used += bufsize;
	qDebug() << used;
	const char *unit[] = {"KB", "MB", "GB"};
	qDebug() << unit[i];
	used += unit[i];
	qDebug() << used << "used";

	system(("/bin/umount " + mntPath).toLatin1());
	system(("/bin/rmdir " + mntPath).toLatin1());

	return true;
}

//k XXX Make sure mntType is valid
//k mount -t mntType /dev/basename mountPointInFStab | /mnt/basename
QString DisksWidget::searchMountInFstab(const QString &device)
{
	QString mnt = "";

	QFile fstab("/etc/fstab");
	if (!fstab.open(QIODevice::ReadWrite | QIODevice::Text)) 
		//k I think won't occur error
		return QString();

	QTextStream ts(&fstab);
	QString line = ts.readLine();
	while (!line.isNull()) {
		if (line.contains(device)) {
			qDebug() << line;
			QStringList list = line.split(QRegExp("\\s+"));
			mnt = list.at(1);
			break;
		}
		line = ts.readLine();
	}

	qDebug() << device << "mount point in fstab " << mnt;
//	getchar();

	//k not in fstab
	if (!mnt.isEmpty())
		system("/bin/mount -a");

	return mnt;
}

//k whether is a primary partition
bool DisksWidget::isPrimary(const QTreeWidgetItem *item)
{
//	QTreeWidgetItem *parent = item->parent();
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		if (item == m_tree->topLevelItem(i))
			return false;
	}

	QTreeWidgetItem *parent = item->parent();

	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		if (parent == m_tree->topLevelItem(i))
			return true;
	}

	return false;
}

//k XXX take care : item must pay attention
//k the INCOMING item should NOT be disk
QString DisksWidget::belongedDisk(QTreeWidgetItem *item)
{
	while (!isPrimary(item))
		item = item->parent();
	return item->parent()->text(colDev);
}

bool DisksWidget::maybeGPT(const QString& disk)
{
	PartitionList *list = m_partitionListMap.value(disk);
    qDebug() << "part table label:" << list->owner_ptable()->type_name();
    return !list->owner_ptable()->is_support_extended_partition();
}

int DisksWidget::primaryCount(const QString &disk)
{
	qDebug() << __FUNCTION__ << " ------ " << disk;
	PartitionList *list = m_partitionListMap.value(disk);

	int count = 0;
	for (int i = 0; i < list->count(); ++i) {
		Partition *part = list->part_index(i);
		//k free partition is "free"
		QString type = part->type_name();
		if (type == "primary" || type == "extended")
			++count;

		if ( QString(part->type_name()) == "extended")
			continue;
	}

	qDebug() << "number is " << count;
	return count;
}

//k only return index of it's parent, should NOT used by disk
int DisksWidget::itemIndex(QTreeWidgetItem *item)
{
	QString disk = belongedDisk(item);
	PartitionList *partList = m_partitionListMap.value(disk);
	int index = 0;
	for ( ; index < partList->count(); ++index) {
		Partition *partition = partList->part_index(index);
		if (QString::number(partition->start()) == item->text(colBeginBlock)
			&& QString::number(partition->end()) == item->text(colEndBlock))
		{
			return index;
		}
	}

	return -1;
}

//k create primary partition, and return index of partition list
int DisksWidget::createPrimary(QTreeWidgetItem *item, const QString &fsType, const QString &length, const QString &type)
{
	QString disk = belongedDisk(item);
	PartitionList *list = m_partitionListMap.value(disk);
	int index = 0;
	Partition *partition = 0;
	for ( ; index < list->count(); ++index) {
		Partition *tmp = list->part_index(index);
		if (QString::number(tmp->start()) == item->text(colBeginBlock)
			&& QString::number(tmp->end()) == item->text(colEndBlock))
		{
			partition = tmp;
			break;
		}
	}
	if (index >= list->count()) {
		qWarning() << __PRETTY_FUNCTION__ << " : Can't find the index";
		return -1;
	}

	int no = 0;
	if (length == "-1") {
		no = list->add_by_whole(index, type.toLatin1(), fsType.toLatin1());
		QList<QString> tmp;
		tmp << belongedDisk(item) << QString::number(index) << type << fsType;
		m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));

		//k find the index
		int index = list->find_index_of(no);
		return index;
	} else {
		no = list->add_by_length(index, type.toLatin1(), fsType.toLatin1(), length.toLatin1(), "MB");

		QList<QString> tmp;
		tmp << belongedDisk(item) << QString::number(index) << type << fsType << length+"MB";
		m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_length", tmp));
		int index = list->find_index_of(no);
		return index;
	}
}

//k call by addPartition when create partition on a free
void DisksWidget::addPartInfo(const int index, QTreeWidgetItem *item, const QString &mntPoint)
{
	QString disk = belongedDisk(item);
	PartitionList *list = m_partitionListMap.value(disk);
	Partition *partition = list->part_index(index);

	PartitionInfo partInfo;
	partInfo.disk = disk;
	partInfo.index = index;
	partInfo.begBlock = QString::number(partition->start());
	partInfo.endBlock = QString::number(partition->end());
	partInfo.mntPoint = mntPoint;
	partInfo.format = true;
	m_partInfoList.push_back(partInfo);
}

bool DisksWidget::existExtended(const QString &disk)
{
	PartitionList *list = m_partitionListMap.value(disk);
	for (int i = 0; i < list->count(); ++i)
		if (list->part_index(i)->type_name() == QString("extended"))
			return true;

	return false;
}

QString DisksWidget::osOnDevice(const QString &device)
{
	char buf[OS_TYPE_LENGTH]; 
	int ret = m_osIdent->get_os_type_str(device.toLatin1(), buf);
	if (ret != -1) {
		qDebug() << buf;
		return QString(buf).trimmed();
	}
	return "";
}

void DisksWidget::writeXML()
{
//	installOnPartition();
//	qDebug() << finalPartitionsInfo();
//	qDebug() << __LINE__;
//
//	QString result = warningInfo();
//	qDebug() << result;
//	qDebug() << __LINE__;

//	QString tmp;
//	qDebug() << validate(tmp);

	Engine *engine = Engine::instance();
	if ( !engine)
		return;

	recordStatus();
	QList<QPair<QString, QList<QString> > >::iterator it;
	for (it = m_commandList.begin(); it != m_commandList.end(); ++it) {
		qDebug() << it->first;
		
		if (it->first == "conf_set_mklabel") {
			QList<QString> tmp = it->second;
			engine->cmdMakeLabel(tmp[0].toLatin1(), tmp[1].toLatin1());
			continue;
		}

		if (it->first == "conf_set_mkpart_whole") {
			QList<QString> tmp = it->second;
			engine->cmdMakePartWhole(tmp[0].toLatin1(), tmp[1].toLatin1(), tmp[2].toLatin1(), tmp[3].toLatin1());
			continue;
		}

		if (it->first == "conf_set_mkpart_length") {
			QList<QString> tmp = it->second;
			engine->cmdMakePartLength(tmp[0].toLatin1(), tmp[1].toLatin1(), tmp[2].toLatin1(), tmp[3].toLatin1(), tmp[4].toLatin1());
			continue;
		}

		if (it->first == "conf_set_rmpart") {
			QList<QString> tmp = it->second;
			engine->cmdRemovePart(tmp[0].toLatin1());
			continue;
		}
			
		if (it->first == "conf_set_mkfs") {
			QList<QString> tmp = it->second;
			engine->cmdMakeFileSystem(tmp[0].toLatin1(), tmp[1].toLatin1());
			continue;
		}
			
		if (it->first == "conf_set_mountpoint") {
			QList<QString> tmp = it->second;
			engine->cmdSetMountPoint(tmp[0].toLatin1(), tmp[1].toLatin1(), tmp[2].toLatin1());
			continue;
		}
	}
}

//k current partition, whatever it is before, transfer to ext4, set /,
void DisksWidget::doSimpleInstall()
{
/* k move to warning info
	QList<OriginalPartsChanges>::iterator it = m_partsChangeList.begin();
	for ( ; it != m_partsChangeList.end(); ++it) {
		if (it->disk == belongedDisk(m_simpleInstallItem)) {
			if (m_simpleInstallItem->text(colBeginBlock) == it->begBlock 
				&& m_simpleInstallItem->text(colEndBlock) == it->endBlock) 
			{
				if (it->original == true) {
					it->original = false;
					it->info = "formatted";
				}
				break;
			}
		}
	}
 */

	int index = itemIndex(m_simpleInstallItem);
	if (index == -1) {
		qWarning() << "get partition index for " << m_simpleInstallItem->text(colDev) << " error.";
		return ;
	}

	QString disk = belongedDisk(m_simpleInstallItem);
	PartitionList *partList = m_partitionListMap.value(disk);
	
	QList<QString> tmp;
	int newPartNo = 0;
	if (m_simpleInstallItem->text(colDev) == "free") { 
		if (isPrimary(m_simpleInstallItem)) {
			//k XXX if the partition is the fourth Primary??
			newPartNo = partList->add_by_whole(index, "primary", 0);
			tmp << disk << QString::number(index) << "primary" << "ext4";
			m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));
		} else {
			newPartNo = partList->add_by_whole(index, "logical", 0);
			tmp << disk << QString::number(index) << "logical" << "ext4";
			m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkpart_whole", tmp));
		}

		if (newPartNo == 0) {
			qWarning() << "create partition by add_by_whole failed. unlikely.";
			return ;
		}
		index = partList->find_index_of(newPartNo);
	}
	
	m_tree->clear();
	rebuildTree();

	Partition *newPartition = partList->part_index(index);
	QString partName = newPartition->path();
	m_simpleInstallItem = 0;
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *diskItem = m_tree->topLevelItem(i);
		if (diskItem->text(colDev) == disk) {
			for (int j = 0; j < diskItem->childCount(); ++j) {
				QTreeWidgetItem *partItem = diskItem->child(j);
				if (partItem->text(colDev) == partName)
				{
					m_simpleInstallItem = partItem;
					break;
				}

				if (partItem->text(colPartType) == "extended") {
					for (int k = 0; k < partItem->childCount(); ++k) {
						QTreeWidgetItem *logicalItem = partItem->child(k);
						if (logicalItem->text(colDev) == partName)
						{
							m_simpleInstallItem = logicalItem;
							break;
						}
					}
				}
			}
			break;
		}
	}

	Q_ASSERT (m_simpleInstallItem !=  0);

	m_simpleInstallItem->setText(colFs, "ext4");
	m_simpleInstallItem->setText(colMount, "/");
	m_simpleInstallItem->setText(colFormat, checkMark);
}

void DisksWidget::recordStatus()
{
	QList<QString> tmp;

	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		//k disk
		QTreeWidgetItem *topItem = m_tree->topLevelItem(i);
		//k every parimary partition
		for (int j = 0; j < topItem->childCount(); ++j) {
			QTreeWidgetItem *child = topItem->child(j);
			qDebug() << __FUNCTION__ << child->text(0);
			doRecord(child);

			if (child->text(colPartType) == "extended") {
				for (int k = 0; k < child->childCount(); ++k)
					doRecord(child->child(k));
			}
		}
	}

	tmp.clear();
	m_commandList.push_back(QPair<QString, QList<QString> >("end_partition_section", tmp));

}

void DisksWidget::doRecord(QTreeWidgetItem *cur)
{
	QList<QString> tmp;
	
	if ( m_mode == Simple && !cur->text(colFormat).isEmpty()) {
		tmp.clear();
		tmp << cur->text(colDev) << cur->text(colFs);
		qDebug() << __FUNCTION__ << cur->text(colDev) << cur->text(colFs);
		m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mkfs", tmp));
	}

	if ( !cur->text(colMount).isEmpty()) {
		tmp.clear();
		tmp << cur->text(colDev) << cur->text(colMount) << cur->text(colFs);
		qDebug() << __FUNCTION__ << cur->text(colDev) << cur->text(colMount) << cur->text(colFs);
		m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mountpoint", tmp));
	}
}

bool DisksWidget::isLinux(const QString &dev)
{
	if (m_osIdent->judge_linux(dev.toLatin1()) == 0)
		return true;

	return false;
	
}

bool DisksWidget::isWindows(const QString &dev)
{
	if (m_osIdent->judge_windows(dev.toLatin1()) == 0) {
		qDebug() << dev << " has a windows";
		return true;
	}

	qDebug() << dev << " does NOT have a windows";
	return false;

}

void DisksWidget::sendPartInfo(QTreeWidgetItem *current, QTreeWidgetItem *) 
{
//	if (m_mode != Linux)
//		return;

//	if ( isPrimary(current))
	if ( !current)
		return ;
	emit currentChanged(current->text(colDev), current->text(colPartType), current->text(colFs));
}

//k simple mode - install on current partition
//k XXX hard to deal
void DisksWidget::installOnPartition()
{
	if (m_mode != Simple)
		return ;

	QTreeWidgetItem *cur = m_tree->currentItem();
	if (cur->text(colPartType).isEmpty() || cur->text(colPartType) == "extended") {
		QMessageBox::warning(this, tr("Warning"), tr("Please select a primary partition or logical partition to install."));
		return;
	}

	m_simpleInstallItem = cur;
}

QString DisksWidget::finalPartitionsInfo()
{
	QString info;
	if (m_mode == Simple) {
		QString partPath = m_simpleInstallItem->text(colDev);
		info = QString(tr("%1 will be formatted to ext4 as / directory\n")).arg(partPath);
	}
	
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *item = m_tree->topLevelItem(i);
		for (int j = 0; j < item->childCount(); ++j) {
			QTreeWidgetItem *child = item->child(j);
			if (child == m_simpleInstallItem)
				continue;
			if ( !child->text(colMount).isEmpty()) {
				QString	line = QString(tr("%1 is %2 file system , and set as %3\n")).arg(child->text(colDev)).arg(child->text(colFs)).arg(child->text(colMount)); 
				info += line;
			}

			if (child->text(colPartType) == "extended") {
				for (int k = 0; k < child->childCount(); ++k) {
					QTreeWidgetItem *grand = child->child(k);
					if (grand == m_simpleInstallItem)
						continue;
					if ( !grand->text(colMount).isEmpty()) {
						QString	line = QString(tr("%1 is %2 file system , and set as %3\n")).arg(grand->text(colDev)).arg(grand->text(colFs)).arg(grand->text(colMount)); 
					
						info += line;
					}
				}
			}
		}
	}

	return info;
}

//k make sure you should have  a root mount point, or return empty 
QString DisksWidget::rootPartitionPath()
{
	QString rootDir;

	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *item = m_tree->topLevelItem(i);
		for (int j = 0; j < item->childCount(); ++j) {
			QTreeWidgetItem *child = item->child(j);
			if ( child->text(colMount) == "/") {
				rootDir = child->text(colDev);
				return rootDir;
			}

			if (child->text(colPartType) == "extended") {
				for (int k = 0; k < child->childCount(); ++k) {
					QTreeWidgetItem *grand = child->child(k);
					if (grand->text(colMount) == "/") {
						rootDir = grand->text(colDev);
						return rootDir;
					}
				}
			}
		}
	}

	return rootDir;
}

QString DisksWidget::warningInfo()
{
	QString result;

	if (m_mode == Simple) {
		QList<OriginalPartsChanges>::iterator it = m_partsChangeList.begin();
		for ( ; it != m_partsChangeList.end(); ++it) {
			if (it->disk == belongedDisk(m_simpleInstallItem)) {
				if (m_simpleInstallItem->text(colBeginBlock) == it->begBlock 
					&& m_simpleInstallItem->text(colEndBlock) == it->endBlock) 
				{
					if (it->original == true) {
						result = QString(tr("%1 will be formatted.\n")).arg(m_simpleInstallItem->text(colDev));
					}
					break;
				}
			}
		}

		return result;
	}

	//k iterate m_partsChangeList, find original == false
	QList<OriginalPartsChanges>::const_iterator it = m_partsChangeList.begin();
	for ( ; it != m_partsChangeList.end(); ++it) {
		if (it->original == false) 
			result += QString(tr("%1 will be %2.\n").arg(it->devPath).arg(it->info));
	}
	
	return result;
}

bool DisksWidget::validate(QString &err, int requiredSizeMB)
{
    int size = targetRootSize();
    if (size < requiredSizeMB) {
        err = tr("You need to select a partition which at least has %1MB!").arg(requiredSizeMB);
        return false;
    } else {
        return true;
    } 

	if (m_mode == Advanced) {
		if (!existMntPoint("/")) {
			err = tr("You have to set a \"/\" mount point.");
			return false;
        }

        if (isEfiEnabled()) {
            if (!existMntPointWithFstype("/boot/efi", "vfat")) {
                err = tr("You need to set a \"/boot/efi\" mount point for a fat32 partition.");
                return false;
            }
        }
	}

	return true; 
}

bool DisksWidget::isEfiEnabled()
{
    struct stat statbuf;
    if (lstat("/sys/firmware/efi", &statbuf) < 0) {
        return false; 
    }

    return S_ISDIR(statbuf.st_mode);
}
/**
 * TODO: what if multiple disks has been modified, what to do?
 * right now, only take care of situation where only one disk's been modified,
 * so / and efi partition are at the same disk.
 */
bool DisksWidget::hasEfipart()
{
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parent = m_tree->topLevelItem(i);
		for (int j = 0; j < parent->childCount(); ++j) {
			QTreeWidgetItem *item = parent->child(j);
			if (item->text(colMount) == "/boot/efi") {
				return true;
			}
		}
	}

    return false;
}

//needs to check only primary partition
bool DisksWidget::existMntPointWithFstype(const QString &mnt, const QString &fstype)
{
    QStringList sl;
    if (fstype == "vfat") {
        sl << "fat32";
        sl << "fat16";
    } else {
        sl << fstype;
    }
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parent = m_tree->topLevelItem(i);
		for (int j = 0; j < parent->childCount(); ++j) {
			QTreeWidgetItem *item = parent->child(j);
			if ((item->text(colMount) == mnt) && sl.indexOf(item->text(colFs)) >= 0) {
				return true;
			}
		}
	}

	return false;
}

//TODO: consider all mountpoints from all disks
//this is gpt-disk compatible
int DisksWidget::targetRootSize()
{
    int target_size = 0;
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parent = m_tree->topLevelItem(i);
		for (int j = 0; j < parent->childCount(); ++j) {
			QTreeWidgetItem *item = parent->child(j);
			if (item->text(colMount) == "/") {
                target_size = humanToSizeMB(item->text(colSize));
			}

			if (item->text(colPartType) == "extended") {
				for (int k = 0; k < item->childCount(); ++k) {
					QTreeWidgetItem *grand = item->child(k);
					if (grand->text(colMount) == "/") {
                        target_size = humanToSizeMB(item->text(colSize));
					}
				}
			}
		}
	}

    return target_size;
}

bool DisksWidget::existMntPoint(const QString &mnt, QTreeWidgetItem *current)
{
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parent = m_tree->topLevelItem(i);
		for (int j = 0; j < parent->childCount(); ++j) {
			QTreeWidgetItem *item = parent->child(j);
			if (item == current)
				continue;
			if (item->text(colMount) == mnt) {
//				found = item;
				return true;
			}

			if (item->text(colPartType) == "extended") {
				for (int k = 0; k < item->childCount(); ++k) {
					QTreeWidgetItem *grand = item->child(k);
					if (grand == current)
						continue;
					if (grand->text(colMount) == mnt) {
//						found = grand;
						return true;
					}
				}
			}
		}
	}

	return false;
}

void DisksWidget::setCurrentIndex(int index)
{
	m_tree->setCurrentItem(m_tree->topLevelItem(index));
}

//k according current position and content, decide enable "New" btn or not
//k 1 disk disable
//k 2 not free disable
//k 3 free is top child, check sum <= 4
//k 4 free in extend, enable
//k MAYBE add set other buttons' state
void DisksWidget::setNEWState(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	previous = previous;

	if (current == NULL) {
		m_add->setEnabled(false);
		return;
	}

	qDebug() << current->text(0) << " -- ";

	bool isDisk = false;
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		if (m_tree->topLevelItem(i) == current) {
			isDisk = true;
			break;
		}
	}

	if (isDisk) {
		m_add->setEnabled(false);
		return ;
	}

	if (current->text(colPartType) != "free") {
		m_add->setEnabled(false);
		return ;
	}

	//k logical and free
	m_add->setEnabled(true);
}

bool DisksWidget::hasWindows()
{
	if (m_tree->topLevelItemCount() != 0)
		return true;

	else
		return false;
}

int DisksWidget::partitionCount(const QString &diskName)
{
	int count = 0;
	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *disk = m_tree->topLevelItem(i);
		if (diskName != disk->text(colDev))
			continue;

		for (int j = 0; j < disk->childCount(); ++j) {
			QTreeWidgetItem *primary = disk->child(j);
			if (primary->text(colPartType) == "primary") 
				++count;

			if (primary->text(colPartType) == "extended") {
				++count;
				for (int k=0; k < primary->childCount(); ++k) {
					if (primary->child(k)->text(colPartType) == "logical")
						++count;
				}
			}
		}

		break;
	}

	return count;
}

int DisksWidget::humanToSizeMB(QString size)
{
    double realSize = 0.0;
	QChar unit = size.at(size.size() - 2);
	if (unit.isLetter()) {
		realSize = size.left(size.size() - 2).toDouble();
		if (unit == 'K' || unit == 'k') {
            realSize /= 1024;
		}

		if (unit == 'G' || unit == 'g') {
            realSize *= 1024;
		}
	} else {
		realSize = size.left(size.size() - 1).toDouble();
	    realSize /= (1024 * 1024);
	}
		
	return realSize;
}

void DisksWidget::create_fs_table(QTreeWidgetItem *current, int )
{
	bool is_disk = false;

	for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
		if (m_tree->topLevelItem(i) == current) {
			is_disk = true;
			break;
		}
	}

	if (!is_disk)
		return ;

	QString disk_name = current->text(colDev);
	PartitionList *partList = m_partitionListMap.value(disk_name);
	if (partList == NULL) {
		if (QMessageBox::Ok != QMessageBox::question(this, tr("Warning"), QString(tr("Do you want to create partition table for %1?").arg(disk_name)), QMessageBox::Ok, QMessageBox::Cancel))
			return ;

		for (int i = 0; i < m_devices->count(); ++i) {
			Device *disk = m_devices->device(i);
			if (!disk)
				continue;

			if (disk_name != disk->path())
				continue;

			PartitionTable *table = disk->parttable();

			//k JiangLi has filtered the read-only device, 
			//k so will never go through if
			if (disk->is_read_only()) {
				QMessageBox::warning(this, tr("Warning"), tr("Can not create partition table on a readonly device."));
				return ;
			} else {
				bool ret = table->create("msdos");
				if (!ret) {
					QMessageBox::warning(this, tr("Warning"), tr("Create partition table failed."));
					return ;
				} else {
					m_partitionListMap.insert(disk_name, table->partlist());
					QList<QString> tmp;
					tmp << disk_name << "msdos";
					m_commandList.push_back(QPair<QString, QList<QString> >("conf_set_mklabel", tmp));
					rebuildTree();
					return ;
				}
			}

			return ;
		}
	}
}
