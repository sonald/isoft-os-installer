#ifndef DISKSWIDGET_H
#define DISKSWIDGET_H

#include <QWidget>
#include <QMap>
#include <QTreeWidget>
#include <parted++/parted++.h>

class QVBoxLayout;
class QTreeWidget;
class PartedDevices;
class QTreeWidgetItem;
class QTreeWidgetItem;
class QPushButton;

class PartitionList;
class OSIdentify;

enum Columns {
	colDev, colPartType, colSize, colUsed, colFs, colOs, colMount, colFormat, colDetail,
	colEmpty, colBeginBlock, colEndBlock
};

const QString checkMark(QChar(0x2713));
const QString AppPath = "/usr/share/libdiskswidget/";
const QString efiMountPoint = "/boot/efi";

class DisksWidget : public QWidget
{
	Q_OBJECT

public:
	enum Modes {Simple, Advanced, Linux, Windows,}; 
	DisksWidget(QWidget *parent = 0, const Modes &model = Simple, QString locale = QString());

	QString		finalPartitionsInfo();
	QString		warningInfo();	
	QString		rootPartitionPath();
	bool		existMntPoint(const QString &mnt, QTreeWidgetItem *current = 0);
	bool		existMntPointWithFstype(const QString &mnt, const QString &fstype);
    int         targetRootSize();
    bool hasEfipart(); // check if efi partition exists in GPT mode
	void		setCurrentIndex(int index);
	QString		currentDevPath();
	QTreeWidgetItem *getCurrentItem() { return m_tree->currentItem(); }

	//k only use for mode _windows
	bool	hasWindows();

	//k really modify the widget item, need to be called right after waning info
	void 	doSimpleInstall();

    int humanToSizeMB(QString size);

	QString osOnDevice(const QString &device);

    bool isEfiEnabled();
    bool maybeGPT(const QString& disk);

public slots:
	void 	reset();
	void	writeXML();

	//k just remember the currentItem
	void 	installOnPartition();

	bool	validate(QString &err, int requiredSizeMB);

signals:
//	void	sigLinuxInfo(QString dev, QString fsType);
	void	currentChanged(QString dev, QString partType, QString fsType);

private:
	void 	initTree();
	void 	rebuildTree();
	bool	calcSize(const QString &dev, const QString &mntType, QString &used);

	//k is it primary partition
	bool 	isPrimary(const QTreeWidgetItem *item);
	int 	primaryCount(const QString &disk);
	int 	itemIndex(QTreeWidgetItem *item);
	int 	createPrimary(QTreeWidgetItem *item, const QString &fsType, const QString &length, const QString &type, PedPartitionFlag flag);
	bool	existExtended(const QString &disk);
	void 	addPartInfo(const int index, QTreeWidgetItem *item, const QString &mntPoint, bool format = false);
	void 	recordStatus();
	bool	isWindows(const QString &dev);
	bool	isLinux(const QString &dev);
	QString	belongedDisk(QTreeWidgetItem *item);
	void	doRecord(QTreeWidgetItem *cur);

private slots:
	void	sendPartInfo(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void 	delPartition();
	void 	addPartition();
	void 	editPartition();
	void	create_fs_table(QTreeWidgetItem *current, int );

	//k set new button enable or not
	void setNEWState(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	//k count current partitions
	int	partitionCount(const QString &diskName);

	//k check /dev/sdxx in fstab and mtab, if exists, return mount path
	//k else return QString()
	QString	searchMountInFstab(const QString &basename);

private:
	Modes		m_mode;
	QTreeWidget 	*m_tree;
	QPushButton 	*m_add;
	QPushButton 	*m_edit;
	QPushButton 	*m_del; 
//	QPushButton 	*m_reset;

	QTreeWidgetItem *m_simpleInstallItem;
	
	PartedDevices	*m_devices;
	QVBoxLayout	*_layout;

	QMap<QString, PartitionList *>		m_partitionListMap; //diskPath, partitionlist
	QList<QPair<QString, QList<QString> > >	m_commandList; // function, parameters

	//k record current  partition's all information
	struct PartitionInfo
	{
		QString disk;		//kk id = disk + index
		//kk index of partition table useless, almost always chenage
		int	index;		
		QString devName;	//kk remember the real path when logical will change
		QString begBlock;
		QString endBlock;
		QString	mntPoint;
		bool 	format;
		QString fsType;		//k fs type after format
        // right now, only special-handle `bios_grub` flag,
        // maybe `boot` in the future.
        QString flag; 
	};
	//k info from user input, without free partition
	QList<PartitionInfo> 		m_partInfoList;

	struct OriginalPartsChanges
	{
		QString disk;
		QString devPath;
		QString	begBlock;
		QString endBlock;
		bool	original;
		QString info;	// devPath will be "formatted" or "deleted"
	};
	//k store all original partitions at initial time
	//k when format or delete it,
	//k search in _partsChangeList and modify its original and info
	QList<OriginalPartsChanges> 	m_partsChangeList;

	//k provides methods to judge linux/windows
	OSIdentify	*m_osIdent;
};

#endif
