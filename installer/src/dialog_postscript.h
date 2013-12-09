#ifndef RFINSTALLER_DIALOG_POSTSCRIPT_H_
#define RFINSTALLER_DIALOG_POSTSCRIPT_H_

#include <QDialog>

class QCloseEvent;
class QHBoxLayout;
class QLabel;
class QMovie;

class DialogPostscript : public QDialog
{
    Q_OBJECT
 public:
    DialogPostscript(QWidget* parent=0);
    ~DialogPostscript();
    void setLabelText();
 protected:
    virtual void closeEvent(QCloseEvent* event);
    
 private:
    QMovie*             m_gif;
    QLabel*             m_labelGif;
    QLabel*		m_labelText;
    QHBoxLayout*	m_layout;
};
#endif // RFINSTALLER_DIALOG_POSTSCRIPT_H_
