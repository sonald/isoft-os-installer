#include "stage_indicator.h"

void StageItem::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    
    int h = height();
    QPoint center(h/2, h/2);
    QRadialGradient gradient(center, h/2, center);
    if (_isPivot) {
        gradient.setColorAt(0, QColor::fromRgbF(1, 1, 0, 1));
        gradient.setColorAt(1, QColor::fromRgbF(1, 0.4, 0, 0.4));
    } else {
        gradient.setColorAt(0, QColor::fromRgbF(1, 0, 1, 1));
        gradient.setColorAt(1, QColor::fromRgbF(1, 0, 0.4, 0.4));
    }
    QBrush brush(gradient);
    p.setBrush(brush);
    p.setPen(Qt::transparent);
    p.drawEllipse(0, 0, h, h);

    p.setPen(Qt::darkRed);
    p.drawText(h, h/2, _description);
}

void StageItem::asPivot(bool set)
{
    if (_isPivot != set) {
        _isPivot = set;
        update();
    }
}

StageIndicator::StageIndicator(QWidget* parent)
    :QWidget(parent)
{
    setWindowOpacity(0.0);
    QStringList stages;
    stages << tr("welcome") << tr("license") << tr("mode select")
        << tr("partition") << tr("summary") << tr("install") 
        << tr("configure") << tr("finish");
    for (int i = 0; i < stages.length(); ++i) {
        addStage(stages[i]);
    }
    adjustItems();
    _current = -1;
}

void StageIndicator::prevStage()
{
    if (_current == 0) return;

    _stages[_current--]->asPivot(false);
    _stages[_current]->asPivot(true);
}

void StageIndicator::nextStage()
{
    if (_current == _stages.size()-1) return;

    if (_current >= 0) {
        _stages[_current++]->asPivot(false);
    } else 
        _current = 0;
    _stages[_current]->asPivot(true);
}

void StageIndicator::addStage(const QString& desc)
{
    int id = _stages.length();
    _stages.append(new StageItem(this, id, desc));
}

void StageIndicator::adjustItems()
{
    int h = 32;
    for (int i = 0; i < _stages.length(); ++i) {
        StageItem *si = _stages[i];
        si->resize(120, h);
        si->move(0, i*h + i*2);
    }
}

void StageIndicator::paintEvent(QPaintEvent *pe)
{
    QWidget::paintEvent(pe);
}
