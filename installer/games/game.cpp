/* vi: set sw=4 ts=4: */
/*
 * game.c: This file is part of ____
 *
 * Copyright (C) 2008 yetist <yetist@gmail.com>
 *
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * */
#include <QDateTime>
#include <QApplication>
#include <QDesktopWidget>
#include "game_mainwindow.h"

static MainWindow *game_window = NULL;

void game_start(void)
{
		qsrand(QDateTime::currentDateTime().toTime_t());

		if (game_window) 
		{
			game_window->show();
			return;
		}
		game_window = new MainWindow();
		//game_window->setFixedSize(780, 460);
		QDesktopWidget* desktop= QApplication::desktop ();
		game_window->resize(desktop->width(), 460);
		game_window->show();
}

void game_destroy(void)
{
		if (game_window) 
		{
			game_window->close();
			game_window->deleteLater();
		}
}

/*
vi:ts=4:wrap:ai:
*/
