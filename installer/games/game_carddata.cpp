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
#include "game_carddata.h"

#include <QPainter>
#include <QDir>
#include <QApplication>
#include <QSvgRenderer>
#include <QtDebug>
#include <QResource>

CardData::CardData(int size)
	: m_size(size)
{
	initBackground(size);
	initImages(size);
}

CardData::~CardData()
{
}

void CardData::initBackground(int size)
{
	m_background=QPixmap(size, size);
	
	QPainter p(&m_background);
	p.fillRect(0, 0, size, size, QColor(255, 255, 255));
	p.setPen(QColor(0, 0, 255));
	p.setBrush(QBrush(Qt::NoBrush));
	p.drawRect(0, 0, size-1, size-1);			
	
	for (int x=10; x<size-10; x++) {
		for (int y=10; y<size-10; y++) {
			if (qrand()%100<50) {
				p.drawPoint(x, y);
			}
		}
	}
	
}

QPixmap CardData::getImage(int nr)
{
	return m_pixmaps.value(nr);	
}

void CardData::initImages(int size)
{	
	
	//QDir dir("/home/qt-devel/projects/Memory/svg");
	//QDir dir(":/svg");
	QString	g_appPath;
	QString g_svg_dir;
	g_appPath = QApplication::applicationDirPath();
	g_svg_dir = g_appPath + "/../share/rfinstaller/svg";
	QDir dir(g_svg_dir);
	QStringList files;
	files=dir.entryList(QStringList("*.svg"), QDir::Files|QDir::Readable);
	m_pixmaps.clear();
	
	foreach (QString file, files) {
		//qDebug() << file;
		QSvgRenderer svg(dir.absolutePath()+"/"+file);
		QPixmap pix(size, size);
		pix.fill();
		QPainter p(&pix);		
		svg.render(&p, QRectF(5, 5, size-10, size-10));				
						
		p.setPen(QColor(0, 0, 255));
		p.setBrush(QBrush(Qt::NoBrush));
		p.drawRect(0, 0, size-1, size-1);
		
		m_pixmaps.append(pix);		
	}
}


