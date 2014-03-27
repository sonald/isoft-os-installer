#ifndef _stage_indicator_h
#define _stage_indicator_h 

#include <QtGui/QtGui>

class StageItem: public QWidget
{
    Q_OBJECT
    public:
        StageItem(QWidget *parent, int stageId, const QString& desc);

        int id() const { return _stageId; }
        QString desc() const { return _description; }
        void asPivot(bool set);

    protected:
        virtual void paintEvent(QPaintEvent *pe);

    private:
        int _stageId;
        QString _description;
        bool _isPivot;
        QPixmap _activePixmap;
        QPixmap _inactivePixmap;
};

class StageIndicator: public QWidget
{
    Q_OBJECT
    public:
        StageIndicator(QWidget* parent = 0);
        void addStage(const QString& desc);
        //make next stage the current
        void nextStage();
        void prevStage();

    protected:
        virtual void paintEvent(QPaintEvent *pe);
        void adjustItems();

    private:
        QList<StageItem*> _stages;
        int _current;
        QPixmap _background;
        QPixmap _completeBar;
        QPixmap _ongoingBar;
        QPixmap _activePixmap;
        QPixmap _completePixmap;
};

#endif
