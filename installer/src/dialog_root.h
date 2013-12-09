#ifndef RFINSTALLER_DIALOG_ROOT_H_
#define RFINSTALLER_DIALOG_ROOT_H_

#include <QDialog>

class QLabel;
class QPushButton;
class QVBoxLayout;

class DialogRoot : public QDialog
{
    Q_OBJECT
 public:
    DialogRoot(QWidget *parent =0);
 private:
    QLabel* 	        m_label;
    QPushButton* 	m_button;
    QVBoxLayout* 	m_layout;
};
#endif // RFINSTALLER_DIALOG_ROOT_H_
