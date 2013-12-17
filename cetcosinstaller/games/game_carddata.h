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
#ifndef CARDDATA_H
#define CARDDATA_H

#include <QList>
#include <QByteArray>
#include <QPixmap>

class CardData {
public:
	CardData(int size);
	~CardData();
	
	int size() const { return m_size; };
	
	QPixmap background() const { return m_background; };
	QPixmap getImage(int nr);
	
protected:
	void initBackground(int size);	
	void initImages(int size);
	
private:
	int m_size;	
	
	QList<QPixmap> m_pixmaps;	
	QPixmap m_background;

};

#endif
