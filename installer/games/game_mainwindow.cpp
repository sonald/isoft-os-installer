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
#include "game_mainwindow.h"
#include "game_scene.h"

#include <QGraphicsView>
#include <QApplication>
#include <QMenuBar>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), m_level(0)
{
	setWindowTitle("Pairs");

	QGraphicsView *view=new QGraphicsView(this);
	setCentralWidget(view);
	
	view->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
	
	m_scene=new Scene(this);
	view->setScene(m_scene);		
	connect(m_scene, SIGNAL(gameReady()), this, SLOT(newLevel()));
	
	setupMenu();
	
	newGame();
}


MainWindow::~MainWindow()
{
}

void MainWindow::setupMenu()
{
	QAction *act;
	QMenu *menu;
	
	menu=menuBar()->addMenu(tr("&File"));
	act=menu->addAction(tr("&New Game"));	
	act->setShortcut(tr("Ctrl+N"));
	connect(act, SIGNAL(triggered()), this, SLOT(newGame()));
	menu->addSeparator();
	act=menu->addAction(tr("&Quit"));
	act->setShortcut(tr("Ctrl+Q"));
	connect(act, SIGNAL(triggered()), this, SLOT(close()));
	/*
	QActionGroup *group=new QActionGroup(this);
	menu=menuBar()->addMenu(tr("&Size"));
	act=menu->addAction(tr("&Small"));
	act->setCheckable(true);
	act->setData(75);
	group->addAction(act);
	act=menu->addAction(tr("&Middle"));
	act->setCheckable(true);
	act->setData(100);
	act->setChecked(true);
	group->addAction(act);
	act=menu->addAction(tr("&Large"));
	act->setCheckable(true);
	act->setData(150);
	group->addAction(act);
	connect(group, SIGNAL(triggered(QAction *)), this, SLOT(setSize(QAction *)));*/
}

void MainWindow::newGame()
{
	m_level=1;
	m_scene->init(m_level);
}

void MainWindow::setSize(QAction *act)
{
	int size=act->data().toInt();
	m_scene->setSize(size);
}

void MainWindow::newLevel()
{
	m_level++;
	
	QString	g_appPath;
	QString g_svg_dir;
	g_appPath = QApplication::applicationDirPath();
	g_svg_dir = g_appPath + "/../share/cetcosinstaller/svg";
	QDir dir(g_svg_dir);
	if (m_level>dir.count()) {	
		m_level=1;
	}	
	m_scene->init(m_level);
	
}

