#include "dashboard.h"
#include "installer_global.h"

DashBoard::DashBoard(QWidget *parent)
    :QGraphicsView(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    //setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    //setWindowFlags(Qt::Desktop);
    this->setAutoFillBackground(false);

    _scene = new QGraphicsScene;
    _prev = NULL;
    _item = NULL;

    this->setScene(_scene);
    this->setFixedSize(qApp->desktop()->rect().size());
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateWith(QPixmap(g_appImgPath + "/installer-background.png"));
}

void DashBoard::updateWith(const QPixmap& pix)
{
    static qreal z = 1000.0;

    adjustPixmap(pix);
    if (_item) {
        _prev = _item;
        //QPixmap blured(_item->boundingRect().size().toSize());
        //QPainter p(&blured);
        //this->render(&p);
        QPixmap blured = QPixmap::grabWidget(this->viewport());
        _prev->setPixmap(blured);


        _item = new QGraphicsPixmapItem;
        _item->setZValue(z);
        z -= 1.0;
        _item->setPixmap(_currentPixmap);
        _item->setPos(0, 0);
        _scene->addItem(_item);
        _item->setGraphicsEffect(createEffect());

        
        QGraphicsOpacityEffect* opaqEffect = new QGraphicsOpacityEffect;
        _prev->setGraphicsEffect(opaqEffect);
        QPropertyAnimation *pa = new QPropertyAnimation(opaqEffect, "opacity");
        pa->setDuration(1500);
        pa->setStartValue(1.0);
        pa->setEndValue(0.1);

        connect(pa, SIGNAL(finished()), this, SLOT(animationFinished()));
        pa->start(QPropertyAnimation::DeleteWhenStopped);

    } else {
        _item = new QGraphicsPixmapItem;
        _item->setZValue(z);
        z -= 1.0;
        _item->setPixmap(_currentPixmap);
        _item->setPos(0, 0);
        _scene->addItem(_item);
        _item->setGraphicsEffect(createEffect());
        _item->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);

        QPropertyAnimation *pa = new QPropertyAnimation(_item->graphicsEffect(), 
                "blurRadius");
        pa->setDuration(2500);
        pa->setStartValue(10);
        pa->setEndValue(40);
        pa->start(QPropertyAnimation::DeleteWhenStopped);
    }
}

void DashBoard::animationFinished()
{
    QPropertyAnimation *pa = new QPropertyAnimation(_item->graphicsEffect(), 
            "blurRadius");
    pa->setDuration(2500);
    pa->setStartValue(10);
    pa->setEndValue(40);
    pa->start(QPropertyAnimation::DeleteWhenStopped);

    if (_prev) {
        _scene->removeItem(_prev);
        delete _prev;
        _prev = NULL;
    }
}

QGraphicsEffect* DashBoard::createEffect()
{
    QGraphicsBlurEffect* effect = new QGraphicsBlurEffect;
    effect->setBlurHints(QGraphicsBlurEffect::AnimationHint);
    effect->setBlurRadius(10);

    return effect;
}

void DashBoard::adjustPixmap(const QPixmap& pix)
{
    if (pix.isNull()) 
        qDebug() << __PRETTY_FUNCTION__ << "pixmap is null";

    QRect r = qApp->desktop()->rect();
    _currentPixmap = pix.scaled(r.width(), r.height(), Qt::IgnoreAspectRatio);
}

