#ifndef RFINSTALLER_PARTITION_AUTOMODE_H_
#define RFINSTALLER_PARTITION_AUTOMODE_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include <parted++/parted++.h>

class PartitionAutoMode : public QObject {
 Q_OBJECT

 public:
    PartitionAutoMode() {} 
    ~PartitionAutoMode() {}

    // try to auto partition the disk, keep the result.
    // return true if can do.
    bool autoPartition();

    QString rootPath()                  { return m_rootpath; }
    QString partitionInfo()             { return m_partinfo; }

    // write the result of partition into xml.
    void writeXML();
    
 private:
    bool autoPartDisk();
    bool autoPartFreePartitions(PartitionList* plist);

    int  findMaxFree(PartitionList* plist);
    int  partExtended(PartitionList* plist, int index);

    int  findExpectFree(PartitionList* plist, int numSwap);
    bool partExpectFree(PartitionList* plist, int index, int numSwap);
    
    bool createExtended(PartitionList* plist, int index);
    bool createSwap(PartitionList* plist, int index, const char* parttype);
    bool createRoot(PartitionList* plist, int index, const char* parttype);

    bool hasSuitFree(PartitionList* plist, long long size);

    // return the number of swap part, 0 when no swap.
    int  numSwapPart(PartitionList* plist);
    const char* swapSizeHint(long long sizeDisk);

    void recordClean();
    void recordLabel(const QString& devpath, const QString& labeltype);
    void recordLength(const QString& devpath, int index, const QString& parttype, 
		      const QString& fstype, const QString& length);
    void recordWhole(const QString& devpath, int index, const QString& parttype, const QString& fstype);
    void recordMountPoint(const QString& partpath, const QString& mountpoint, const QString& fstype);

    QStringList m_result;
    QString m_rootpath;
    QString m_partinfo;
    
    Device* m_dev;
};

#endif // RFINSTALLER_PARTITION_AUTOMODE_H_
