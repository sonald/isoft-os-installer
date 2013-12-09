#ifndef RFINSTALLER_DIALOG_SUMMARY_H_
#define RFINSTALLER_DIALOG_SUMMARY_H_

#include <QDialog>

class QCloseEvent;
class QGroupBox;
class QHBoxLayout;
class QPushButton;
class QRadioButton;
class QSpacerItem;
class QVBoxLayout;

class DialogSummary : public QDialog
{
    Q_OBJECT
 public:
    DialogSummary( QWidget* parent =0, bool install =true, bool mbr =true);
    ~DialogSummary() {}

    void setInstall(bool install);
    void setMBR(bool mbr);
    bool isInstall();
    bool isMBR();

 public slots:
    virtual void accept();

 private:
    QRadioButton* m_mbr;
    QRadioButton* m_root;
    QGroupBox*    m_groupBox;
    QVBoxLayout*  m_groupLayout;
    QPushButton*  m_ok;
    QPushButton*  m_cancel;
    QSpacerItem*  m_spacerItem;
    QHBoxLayout*  m_buttonLayout;
    QVBoxLayout*  m_formLayout;

    bool m_flagMBR;
    bool m_flagInstall;
};

#endif // RFINSTALLER_DIALOG_SUMMARY_H_
