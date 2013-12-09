#ifndef RFINSTALLER_DIALOG_REJECT_H_
#define RFINSTALLER_DIALOG_REJECT_H_

#include <QDialog>

class QHBoxLayout;
class QLabel;
class QPushButton;
class QSpacerItem;
class QVBoxLayout;

class DialogReject : public QDialog
{
    Q_OBJECT
public:
    DialogReject(QWidget* parent =0);
    void setWarning( const QString& warn );

private:
    QLabel*      m_warning;
    QLabel*      m_quest;
    QSpacerItem* m_spacer;
    QPushButton* m_ok;
    QPushButton* m_cancel;
    QHBoxLayout* m_layoutButton;
    QVBoxLayout* m_layoutForm;
};
#endif // RFINSTALLER_DIALOG_REJECT_H_
