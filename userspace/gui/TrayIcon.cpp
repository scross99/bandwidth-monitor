#include <sstream>
#include <string>

#include <iostream>

#include <QtGui>

#include <common/Bandwidth.hpp>
#include <common/Display.hpp>
#include <common/ULong.hpp>

#include <gui/TrayIcon.h>
#include <gui/Window.h>

TrayIcon::TrayIcon(Window& window) : window_(window){
	QAction * quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	
	QMenu * trayIconMenu = new QMenu();
	trayIconMenu->addAction(quitAction);
	
	setContextMenu(trayIconMenu);
	
	setToolTip(tr("Bandwidth Monitor"));
	
	QIcon icon(":/icon.svg");
	
	setIcon(icon);
	show();
	
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void TrayIcon::iconActivated(QSystemTrayIcon::ActivationReason reason) {
	switch(reason) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
		case QSystemTrayIcon::MiddleClick:
			window_.setVisible(!window_.isVisible());
			break;
		default:
			;
	}
}

void TrayIcon::update(const Bandwidth& bandwidth){
	const ULong totalUsage = bandwidth.down + bandwidth.up;

	std::ostringstream stream;
	stream << "Total: " << displayDataSize(totalUsage) << ", Down: " << displayDataSize(bandwidth.down) << ", Up: " << displayDataSize(bandwidth.up);
	setToolTip(tr(stream.str().c_str()));
}

