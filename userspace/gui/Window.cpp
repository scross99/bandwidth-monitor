#include <ctime>
#include <sstream>
#include <string>
#include <QtGui>

#include <common/Bandwidth.hpp>
#include <common/BandwidthFile.hpp>
#include <common/Display.hpp>
#include <common/Module.hpp>
#include <common/ULong.hpp>

#include <gui/TrayIcon.h>
#include <gui/Window.h>

Window::Window() : trayIcon_(*this){
	progress_ = new QProgressBar(this);
	progress_->setMinimum(0);
	progress_->setMaximum(100000);
	
	label_ = new QLabel(this);
	
	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->addWidget(progress_);
	layout->addWidget(label_);
	setLayout(layout);

	setWindowTitle(tr("Bandwidth Monitor"));

	const time_t currentTime = time(0);
	const tm * timeData = localtime(&currentTime);
	prevWeekDay_ = timeData->tm_wday;

	QTimer * timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(100);
	
	BandwidthFile file;
	
	Module module;
	seconds_ = 0ul;
	file.load(&startBandwidth_);
	moduleStartBandwidth_ = module.getBandwidth();
}

Window::~Window(){
	//...
}

void Window::closeEvent(QCloseEvent * event){
	hide();
	event->ignore();
}

void Window::update(){
	Module module;

	const ULong seconds = module.getSeconds();
	
	if(seconds != seconds_){
		seconds_ = seconds;
		const Bandwidth moduleBandwidth = module.getBandwidth();

		const time_t currentTime = time(0);
		const tm * timeData = localtime(&currentTime);
		const int currentWeekDay = timeData->tm_wday;

		if(currentWeekDay != prevWeekDay_){
			prevWeekDay_ = currentWeekDay;
			moduleStartBandwidth_ = moduleBandwidth;
			startBandwidth_ = Bandwidth(0ul, 0ul);
		}
	
		const Bandwidth bandwidth = (moduleBandwidth - moduleStartBandwidth_) + startBandwidth_;

		const ULong total = bandwidth.down + bandwidth.up;
		const ULong limit = 4ul * 1024ul * 1024ul * 1024ul; // 4GB.
		const ULong maxTransferRate = (limit > total ? limit - total : 0ul) / 60ul;
	
		BandwidthFile file;
		file.save(bandwidth);
	
		module.setLimit(maxTransferRate);
		
		const double usage = total < limit ? double(total) / double(limit) : 1.0;

		progress_->setValue(usage * 100000.0);
	
		std::ostringstream stream;
		stream << "Bandwidth Limit: " << displayDataSize(limit) << std::endl;
		stream << "Bandwidth Used (Total): " << displayDataSize(total) << std::endl;
		stream << "Bandwidth Used (Down): " << displayDataSize(bandwidth.down) << std::endl;
		stream << "Bandwidth Used (Up): " << displayDataSize(bandwidth.up) << std::endl;
		stream << "Current max transfer rate: " << displayDataSize(maxTransferRate) << "/s";
	
		label_->setText(tr(stream.str().c_str()));

		trayIcon_.update(bandwidth);
	}
}

