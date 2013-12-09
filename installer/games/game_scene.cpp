/***************************************************************************
 *   Copyright (C) 2008 by Weinzierl Stefan                                *
 *   Stefan@Weinzierl-Stefan.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "game_scene.h"
#include "game_card.h"

#include <QPainter>
#include <QApplication>
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QtDebug>
#include <QTimer>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <QDir>

Scene::Scene(QObject *parent)
 	: QGraphicsScene(parent), m_size(50), m_zPos(1)
{
	
	m_timer=new QTimer(this);
	m_timer->setSingleShot(true);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(resetCards()));
	
	int s=80;
	QPixmap bg(s, s);
	
	QPainter p(&bg);
	p.setPen(QColor(0, 0, 255));
	p.setBrush(QBrush(Qt::NoBrush));	
	
	for (int x=0; x<s; x++) {
		for (int y=0; y<s; y++) {
			QColor c;
			c.setHsv(120-qrand()%50, qrand()%55+200, qrand()%55+200);
			p.setPen(c);			
			p.drawPoint(x, y);			
		}
	}
	
	setBackgroundBrush(bg);	
}

Scene::~Scene()
{
}

void Scene::init(int count)
{
	foreach (QGraphicsItem *item, items()) {
		delete item;
	}
	//clear();
	m_show.clear();
	m_timer->stop();
	m_zPos=1;
	
	//QDir dir(":/svg");
	QString	g_appPath;
	QString g_svg_dir;
	g_appPath = QApplication::applicationDirPath();
	g_svg_dir = g_appPath + "/../share/rfinstaller/svg";
	QDir dir(g_svg_dir);
	int anz=dir.count();
	
	QList<int> cards;
	int r;
	for (int i=0; i<count; i++) {
		do {
			r=qrand()%anz;
		} while (cards.contains(r));		
		cards.append(r);
		cards.append(r);		
	}	
	
	int x1=(int)ceil(sqrt(cards.count()));
	int y1=cards.count()/x1;
	
	for (int y=0; y<=y1; y++) {
		for (int x=0; x<x1; x++) {		
			if (!cards.empty()) {
				Card *card=new Card();
				addItem(card);		
				card->setPos(x*(m_size+10), y*(m_size+10));
				card->show();		
				card->setNr(cards.takeAt(qrand()%cards.count()));
			}
		}
	}
	
}

void Scene::setSize(int size)
{
	m_size=size;
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	Card *card=dynamic_cast<Card*>(itemAt(mouseEvent->scenePos()));
	if (card && m_show.count()<2 && !card->discover()) {
		card->setDiscover(true);
		m_show.append(card);
		
		if (m_show.count()==2) {
			if (m_show.value(0)->nr()==m_show.value(1)->nr()) {				
				
				foreach (Card *card, m_show) {
					QGraphicsItemAnimation *ani=new QGraphicsItemAnimation(this);
					ani->setPosAt(0, card->pos());
					ani->setPosAt(1, QPointF(-110-m_zPos, m_zPos*10));				
					ani->setRotationAt(0, 0);
					ani->setRotationAt(0.5, 180);
					ani->setRotationAt(1.0, 360);
					
					QTimeLine *time=new QTimeLine(1000, this);
					time->setFrameRange(0, 100);					
					card->setZValue(m_zPos++);
					ani->setItem(card);
					ani->setTimeLine(time);
					time->start();
				}
				
				m_show.clear();
				checkReady();
			} else {
				m_timer->start(1000);
			}
		}		
	}
}

void Scene::resetCards()
{
	foreach (Card *card, m_show) {
		card->setDiscover(false);		
	}
	m_show.clear();
}

void Scene::checkReady()
{	
	foreach (QGraphicsItem *item, items()) {
		Card *card=dynamic_cast<Card*>(item);
		if (card && !card->discover()) {
			return;
		}
	}
			
	QTimer::singleShot(2000, this, SIGNAL(gameReady()));
}



