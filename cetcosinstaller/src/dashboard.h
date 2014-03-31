#ifndef _dashboard_h
#define _dashboard_h 

#include <QtGui>

class DashBoard: public QGraphicsView
{
    Q_OBJECT
    public:
        DashBoard(QWidget *parent = 0);
        void updateWith(const QPixmap& pix);

    protected slots:
        void animationFinished();

    private:
        QGraphicsScene* _scene;
        QGraphicsPixmapItem* _item;
        QGraphicsPixmapItem* _prev;
        QPixmap _currentPixmap;

        void adjustPixmap(const QPixmap& pix);
        QGraphicsEffect* createEffect();
};

#endif
