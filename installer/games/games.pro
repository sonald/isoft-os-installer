TEMPLATE = lib
QT += svg
TARGET = rfinsgame

HEADERS += 			\
	game_card.h 		\
	game_mainwindow.h 	\
	game_carddata.h 	\
	game_scene.h		\
	game.h

SOURCES +=			\
	game_mainwindow.cpp 	\
	game_carddata.cpp 	\
	game_scene.cpp 		\
	game_card.cpp		\
	game.cpp

target.path=/usr/lib

INSTALLS += target
