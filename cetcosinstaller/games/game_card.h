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
#ifndef CARD_H
#define CARD_H

#include <QObject>
#include <QGraphicsItem>
#include <QPixmap>

class CardData;

class Card : public QObject, public QGraphicsItem
{
public:
	Card();
	~Card();
	
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	
	void setNr(int nr);
	int nr() const { return m_nr; };
	
	void setDiscover(bool d);
	bool discover() const { return m_show; };
	
	
protected:	
	void timerEvent(QTimerEvent *event);
	
public slots:
	void animate();
	

	
protected:
	CardData *m_data;
	int m_nr;
	bool m_show;
	int m_animate;
	int m_timerId;
	int m_size;
};

#endif
