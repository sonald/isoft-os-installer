#include "stage_indicator.h"

void StageItem::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    
    int h = height();
    QPoint center(h/2, h/2);
    QRadialGradient gradient(center, h/2, center);
    gradient.setColorAt(0, QColor::fromRgbF(1, 1, 0, 1));
    gradient.setColorAt(1, QColor::fromRgbF(1, 0.4, 0, 0.4));
    QBrush brush(gradient);
    p.setBrush(brush);
    p.setPen(Qt::transparent);
    p.drawEllipse(0, 0, h, h);

    p.setPen(Qt::darkRed);
    p.drawText(h, h/2, _description);
}

StageIndicator::StageIndicator(QWidget* parent)
    :QWidget(parent)
{
    setWindowOpacity(0.0);
    QStringList stages;
    stages << "welcome" << "mode" << "part";
    for (int i = 0; i < stages.length(); ++i) {
        addStage(stages[i]);
    }
    adjustItems();
}

void StageIndicator::addStage(const QString& desc)
{
    int id = _stages.length();
    _stages.append(new StageItem(this, id, desc));
}

void StageIndicator::adjustItems()
{
    int h = 48;
    for (int i = 0; i < _stages.length(); ++i) {
        StageItem *si = _stages[i];
        si->resize(100, h);
        si->move(0, i*h + i*2);
    }
}

void StageIndicator::paintEvent(QPaintEvent *pe)
{
    QWidget::paintEvent(pe);
}
