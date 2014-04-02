#include "installer_global.h"
#include "stage_indicator.h"

StageItem::StageItem(QWidget *parent, int stageId, const QString& desc)
    :QWidget(parent), _stageId(stageId), _description(desc), _isPivot(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    _activePixmap = QPixmap(g_appImgPath + "/ongoingNode.png");
    _inactivePixmap = QPixmap(g_appImgPath + "/completedNode.png");
    setFixedSize(28, 28);
}

void StageItem::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    
    if (_activePixmap.isNull()) {
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
    } else {
        //p.drawPixmap(0, 0, _isPivot?_activePixmap:_inactivePixmap);
    }
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
    _background = QPixmap(g_appImgPath + "/progressBackground.png");
    _completeBar = QPixmap(g_appImgPath + "/completedGantt.png");;
    _ongoingBar = QPixmap(g_appImgPath + "/ongoingGantt.png");;
    _activePixmap = QPixmap(g_appImgPath + "/ongoingNode.png");
    _completePixmap = QPixmap(g_appImgPath + "/completedNode.png");

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
    setFixedSize(32, 414);
}

void StageIndicator::prevStage()
{
    if (_current == 0) return;

    _stages[_current--]->asPivot(false);
    _stages[_current]->asPivot(true);
    update();
}

void StageIndicator::nextStage()
{
    if (_current == _stages.size()-1) return;

    if (_current >= 0) {
        _stages[_current++]->asPivot(false);
    } else 
        _current = 0;
    _stages[_current]->asPivot(true);
    update();
}

void StageIndicator::addStage(const QString& desc)
{
    int id = _stages.length();
    _stages.append(new StageItem(this, id, desc));
}

void StageIndicator::adjustItems()
{
    int h = 28;
    for (int i = 0; i < _stages.length(); ++i) {
        StageItem *si = _stages[i];
        si->move(-1, -1);
        si->move(2, 2 + i*h + i*27);
    }
}

void StageIndicator::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);

    QFont f("mono", 12, QFont::Black, false);
    p.setPen(Qt::white);
    p.setFont(f);
    QFontMetrics fm(f);

    p.drawPixmap(0, 0, _background);
    int h = 28;
    for (int i = 0; i <= _current; ++i) {
        QPoint pos(2, 2 + i*h + i*27);

        //HACK: material image have problem
        if (i == 5) pos.ry() -= 1;
        else if (i == 6) pos.ry() -= 2;
        else if (i == 7) pos.ry() -= 4;

        if (i < _current) {
            p.drawPixmap(pos, _completePixmap);
        } else if (i == _current)
            p.drawPixmap(pos, _activePixmap);

        QRect r = fm.boundingRect(QString::number(i+1));
        p.drawText((32-r.width())/2, pos.y() + (h+fm.ascent())/2,
                QString::number(i+1));

        if (i) {
            QPoint pos_bar((32-3)/2, pos.y() - 28);
            if (i == _current) {
                //HACK: material image have problem
                if (i == 5) pos_bar.ry() += 1;
                else if (i == 6) pos_bar.ry() += 2;
                else if (i == 7) pos_bar.ry() += 3;

                p.drawPixmap(pos_bar, _ongoingBar);
            } else 
                p.drawPixmap(pos_bar, _completeBar);
        }
    }
}
