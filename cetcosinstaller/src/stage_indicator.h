#ifndef _stage_indicator_h
#define _stage_indicator_h 

#include <QtGui/QtGui>

class StageItem: public QWidget
{
    Q_OBJECT
    public:
        StageItem(QWidget *parent, int stageId, const QString& desc)
            :QWidget(parent), _stageId(stageId), _description(desc), _isPivot(false)
        {
            setAttribute(Qt::WA_TranslucentBackground);
        }

        int id() const { return _stageId; }
        QString desc() const { return _description; }
        void asPivot(bool set);

    protected:
        virtual void paintEvent(QPaintEvent *pe);

    private:
        int _stageId;
        QString _description;
        bool _isPivot;
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
};

#endif
