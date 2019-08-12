/*************************************** 
* 
* LanXin TECH, All Rights Reserverd. 
* Created at Mon Aug 12 14:50:45 2019
* Contributor: Ling Shi, Ph.D 
* Email: lshi@robvision.cn 
* 
***************************************/ 


#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>

#include "src/xlabel.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QDialog wgt;
	wgt.setMouseTracking(true);
	QVBoxLayout* layout = new QVBoxLayout(&wgt);
	wgt.setLayout(layout);

	auto xlabel = new XLabel(&wgt);
	layout->addWidget(xlabel);
	layout->addStretch();

	QImage img("../logo_s.png");
	xlabel->setImageMap(img);

	wgt.resize(640, 480);
	wgt.show();


	return a.exec();
}