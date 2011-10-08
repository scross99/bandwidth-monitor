#ifndef TRAYICON_H
#define TRAYICON_H

#include <string>

#include <QSystemTrayIcon>

#include <common/Bandwidth.hpp>

class Window;

class QAction;
class QMenu;

class TrayIcon : public QSystemTrayIcon {
		Q_OBJECT

	public:
		TrayIcon(Window& window);
		
		void update(const Bandwidth& bandwidth);

	private slots:
		void iconActivated(QSystemTrayIcon::ActivationReason reason);

	private:
		Window& window_;

};

#endif
