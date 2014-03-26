#ifndef _stage_indicator_h
#define _stage_indicator_h 

#include <QtGui/QtGui>

class StageItem: public QWidget
{
    Q_OBJECT
    public:
        StageItem(QWidget *parent, int stageId, const QString& desc)
            :QWidget(parent), _stageId(stageId), _description(desc) 
        {
            setAttribute(Qt::WA_TranslucentBackground);
        }

        int id() const { return _stageId; }
        QString desc() const { return _description; }

    protected:
        virtual void paintEvent(QPaintEvent *pe);

    private:
        int _stageId;
        QString _description;
};

class StageIndicator: public QWidget
{
    Q_OBJECT
    public:
        StageIndicator(QWidget* parent = 0);
        void addStage(const QString& desc);


    protected:
        virtual void paintEvent(QPaintEvent *pe);
        void adjustItems();

    private:
        QList<StageItem*> _stages;
};

#endif
