#include <cstdio>
#include <ctime>
#include <unistd.h>

#include <QtGui>

#include <gui/TrayIcon.h>
#include <gui/Window.h>

int main(int argc, char * argv[]) {
	QApplication app(argc, argv);
	
	// Starting up: system tray may take a few seconds to appear.
	for(unsigned int i = 0; i < 60; i++){
		if(QSystemTrayIcon::isSystemTrayAvailable()) {
			break;
		}else{
			sleep(1);
		}
	}

	if(!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Bandwidth Monitor"), QObject::tr("No system tray detected"));
		return 1;
	}

	Window window;
	return app.exec();
}

