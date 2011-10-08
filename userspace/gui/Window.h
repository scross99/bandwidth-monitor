#ifndef WINDOW_H
#define WINDOW_H

#include <QtGui>

#include <common/Bandwidth.hpp>
#include <common/ULong.hpp>

#include <gui/TrayIcon.h>

class Window: public QDialog{
	Q_OBJECT

	public:
		Window();

		virtual ~Window();

	protected:
		virtual void closeEvent(QCloseEvent * event);

	private slots:
		void update();

	private:
		QLabel * label_;
		QProgressBar * progress_;
		TrayIcon trayIcon_;
		int prevWeekDay_;
		ULong seconds_;
		Bandwidth startBandwidth_, moduleStartBandwidth_;

};

#endif
