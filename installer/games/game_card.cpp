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
#include "game_card.h"
#include "game_carddata.h"

#include <QPainter>
#include <QtDebug>
#include <QTimerEvent>
#include <QGraphicsSceneMouseEvent>

Card::Card()
 	: QObject(), QGraphicsItem(), m_nr(0), m_show(false), m_animate(25), m_timerId(0), m_size(50)
{	
	static CardData *cd=0;
	if (!cd) {
		cd=new CardData(m_size);
	}
	m_data=cd;
}

Card::~Card()
{
}

QRectF Card::boundingRect() const
{
	QRectF rect(-m_size/2, -m_size/2, m_size, m_size);
	rect.adjust(-5, -5, 5, 5);
	return rect;//translated(x(), y());
}

void Card::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	painter->save();
				
	painter->scale(1, m_animate/25.0);
		
	painter->setPen(QPen(Qt::NoPen));
	painter->setBrush(QColor(255, 255, 255, 128));
	painter->fillRect(-m_data->size()/2+5, -m_data->size()/2+5, m_data->size(), m_data->size(), QColor(0, 0, 0, 128));
	
	if (m_show) { 	
		painter->drawPixmap(-m_data->size()/2, -m_data->size()/2, m_data->getImage(m_nr));
	} else {
		painter->drawPixmap(-m_data->size()/2, -m_data->size()/2, m_data->background());	
	}
	
	painter->restore();
}

void Card::setNr(int nr)
		
{
	m_nr=nr;
	update();
}

void Card::timerEvent(QTimerEvent *event) 
{		
	if (event->timerId()==m_timerId) {
	 	if (m_animate<25) {
			m_animate++;				
			update();
		} else {
			killTimer(m_timerId);
		}
	}
}

void Card::setDiscover(bool d)
{
	if (d!=m_show) {
		m_show=d;
		m_animate=0;
			
		m_timerId=startTimer(3);	
			
		update();
	}
}

