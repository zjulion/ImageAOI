/*************************************** 
* 
* LanXin TECH, All Rights Reserverd. 
* Contributor: Ling Shi, Ph.D 
* Email: lshi@robvision.cn 
* 
***************************************/ 
#include "xlabel.h"

#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

 
static bool isPointNearSide(const int _sideCoordinate, const int _pointCoordinate)
{
	static const int indent = 6;
	return abs(_sideCoordinate - _pointCoordinate ) <indent  && abs(_pointCoordinate - _sideCoordinate) < indent;
}

XLabel::XLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent), selection_mode(false)
{
	scale = 0.99f;
	mouse_pressed = false;
	setMouseTracking(true);
}

XLabel::~XLabel() {}

static const QSize WIDGET_MINIMUM_SIZE(32, 32);

void XLabel::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button())
	{
		mouse_pressed = true;
		start_mouse_pos = originPoint(event->pos());
		last_static_rect = croppingRect;
	}

	updateCursorIcon(originPoint(event->pos()));
}

void XLabel::mouseMoveEvent(QMouseEvent* event)
{
	auto origin_p = originPoint(event->pos());

	if (!mouse_pressed)
	{
		cursor_pos = cursorPosition(croppingRect, origin_p);

		updateCursorIcon(origin_p);
	}
	else if (cursor_pos != CursorPositionUndefined)
	{
		QPointF mouseDelta;
		mouseDelta.setX(origin_p.x() - start_mouse_pos.x());
		mouseDelta.setY(origin_p.y() - start_mouse_pos.y());

		int dx = WIDGET_MINIMUM_SIZE.width() / 2;
		int dy = WIDGET_MINIMUM_SIZE.height() / 2;
		//
		if (cursor_pos != CursorPositionMiddle) 
		{
			QRectF newGeometry =
				calculateGeometry(
					last_static_rect,
					cursor_pos,
					mouseDelta);
		
			if (!newGeometry.isNull()) 
			{

				// boudary check
				if (newGeometry.x() < image.width() - dx && newGeometry.y () < image.height() - dy 
					&& newGeometry.width() + newGeometry.x() > dx
					&& newGeometry.height() + newGeometry.y() > dy)
				{
					if (newGeometry.width() >= WIDGET_MINIMUM_SIZE.width() && newGeometry.height() >= WIDGET_MINIMUM_SIZE.height())
					{
						croppingRect = newGeometry;
					}
				}

			}
		}
		else
		{
			auto new_pt = last_static_rect.topLeft() + mouseDelta;
			if (new_pt.x() < image.width() - dx && new_pt.y() < image.height() - dy
				&& croppingRect.width() + new_pt.x() > dx
				&& croppingRect.height() + new_pt.y() > dy)
			{
				croppingRect.moveTo(last_static_rect.topLeft() + mouseDelta);
			}
		}

		update();
		
	}


}

void XLabel::mouseReleaseEvent(QMouseEvent* event) 
{
	mouse_pressed = false;
	updateCursorIcon(originPoint(event->pos()));

	// single-click signal
    //emit clicked(int(0.5 + event->x()/scale), int(0.5+event->y()/scale));
	printf("[ %d, %d] \n", (int)event->x(), (int)event->y());
}

void XLabel::wheelEvent(QWheelEvent * e)
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;

	float k = numDegrees > 0 ? 1.09 : 0.90;

	k = k *scale;

	setScale(k );
 
}

void XLabel::mouseDoubleClickEvent(QMouseEvent * event)
{
	
	selection_mode = !selection_mode;
	update();
}


void XLabel::paintEvent(QPaintEvent * _event)
{	
	QLabel::paintEvent(_event);

	if (image.isNull()) return;
	int width = image.width();
	

	//printf("Selection: %d\n", (int)selection_mode);


	QPixmap rawImage = QPixmap::fromImage(image);
	QPainter widgetPainter;
	widgetPainter.begin(&rawImage);

	// Image boundary
	QRectF rect(2, 2, image.width()-4, image.height()-4);
	QPen pen0(Qt::darkRed);
	if (selection_mode)
	{
		pen0 = QPen(Qt::darkGreen);
	}
	pen0.setWidth(4);
	widgetPainter.setPen(pen0);
	widgetPainter.drawRect(rect);


	if (selection_mode)
	{
		if (croppingRect.isNull()) {
			const int width = image.width() / 2  - 4;
			const int height = image.height() / 2 - 4;
			croppingRect.setSize(QSize(width, height));

			float x = (image.width() - croppingRect.width()) / 2-2;
			float y = (image.height() - croppingRect.height()) / 2-2;
			croppingRect.moveTo(x, y);
		}

		//qDebug() << "H: " << croppingRect.height();


		// 1. bg color
		widgetPainter.setBrush(QBrush(QColor(0, 0x6f, 0, 120)));
		QPen pen;
		pen.setColor(Qt::yellow);
		pen.setWidth(2);
		widgetPainter.setPen(pen);
		
		widgetPainter.drawRect(croppingRect);
		// pos
		widgetPainter.setPen(Qt::red);
		QFont font;
		font.setPointSize(croppingRect.width() >240 ? 16 : 10);
		widgetPainter.setFont(font);
		auto tl = croppingRect.topLeft();
		widgetPainter.drawText(QPoint(tl.x() + 6, tl.y() + 24), QString("(%1, %2, %3, %4)").arg(croppingRect.x()).arg(croppingRect.y())\
			.arg(croppingRect.width()).arg(croppingRect.height()));

		// 2. corner boxes
		{
			widgetPainter.setPen(Qt::green);
			widgetPainter.setBrush(QBrush(Qt::white));
			const int edgelen = 8;

			// Вспомогательные X координаты
			int leftXCoord = croppingRect.left() - edgelen/2+1;
			int centerXCoord = croppingRect.center().x() - edgelen / 2;
			int rightXCoord = croppingRect.right() - edgelen / 2+1;
			// Вспомогательные Y координаты
			int topYCoord = croppingRect.top() - edgelen / 2+1;
			int middleYCoord = croppingRect.center().y() - edgelen / 2;
			int bottomYCoord = croppingRect.bottom() - edgelen / 2+1;
			//
			const QSize pointSize(edgelen, edgelen);
			//
			QVector<QRect> points;
			points
				// левая сторона
				<< QRect(QPoint(leftXCoord, topYCoord), pointSize)
				<< QRect(QPoint(leftXCoord, middleYCoord), pointSize)
				<< QRect(QPoint(leftXCoord, bottomYCoord), pointSize)
				// центр
				<< QRect(QPoint(centerXCoord, topYCoord), pointSize)
				<< QRect(QPoint(centerXCoord, middleYCoord), pointSize)
				<< QRect(QPoint(centerXCoord, bottomYCoord), pointSize)
				// правая сторона
				<< QRect(QPoint(rightXCoord, topYCoord), pointSize)
				<< QRect(QPoint(rightXCoord, middleYCoord), pointSize)
				<< QRect(QPoint(rightXCoord, bottomYCoord), pointSize);
			//
			widgetPainter.drawRects(points);
		}



		// 3. center dash lines
		{
			QPen dashPen(Qt::white);
			dashPen.setStyle(Qt::DashLine);
			widgetPainter.setPen(dashPen); 

			widgetPainter.drawLine(
				QPoint(croppingRect.center().x(), croppingRect.top()),
				QPoint(croppingRect.center().x(), croppingRect.bottom()));
			// ... горизонтальная
			widgetPainter.drawLine(
				QPoint(croppingRect.left(), croppingRect.center().y()),
				QPoint(croppingRect.right(), croppingRect.center().y()));
		}
		

	}



	widgetPainter.end();
	this->setPixmap(rawImage.scaledToWidth(scale*width)); 
		
}


CursorPosition XLabel::cursorPosition(const QRectF & _cropRect, const QPointF & _mousePosition)
{
	CursorPosition cursorPosition = CursorPositionUndefined;
	//
	if (1 ) {
		// Двухстороннее направление
		if (isPointNearSide(_cropRect.top(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.left(), _mousePosition.x())) {
			cursorPosition = CursorPositionTopLeft;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.left(), _mousePosition.x())) {
			cursorPosition = CursorPositionBottomLeft;
		}
		else if (isPointNearSide(_cropRect.top(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.right(), _mousePosition.x())) {
			cursorPosition = CursorPositionTopRight;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.right(), _mousePosition.x())) {
			cursorPosition = CursorPositionBottomRight;
			// Одностороннее направление
		}
		else if (isPointNearSide(_cropRect.left(), _mousePosition.x())) {
			cursorPosition = CursorPositionLeft;
		}
		else if (isPointNearSide(_cropRect.right(), _mousePosition.x())) {
			cursorPosition = CursorPositionRight;
		}
		else if (isPointNearSide(_cropRect.top(), _mousePosition.y())) {
			cursorPosition = CursorPositionTop;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y())) {
			cursorPosition = CursorPositionBottom;
			// Без направления
		}
		else {
			if(_cropRect.contains(_mousePosition))
				cursorPosition = CursorPositionMiddle;
		}
	}
	//
	return cursorPosition;
}
void XLabel::updateCursorIcon(const QPointF & _mousePosition)
{

	QCursor cursorIcon;
	//
	switch (cursorPosition(croppingRect, _mousePosition))
	{
		case CursorPositionTopRight:
		case CursorPositionBottomLeft:
			cursorIcon = QCursor(Qt::SizeBDiagCursor);
			break;
		case CursorPositionTopLeft:
		case CursorPositionBottomRight:
			cursorIcon = QCursor(Qt::SizeFDiagCursor);
			break;
		case CursorPositionTop:
		case CursorPositionBottom:
			cursorIcon = QCursor(Qt::SizeVerCursor);
			break;
		case CursorPositionLeft:
		case CursorPositionRight:
			cursorIcon = QCursor(Qt::SizeHorCursor);
			break;
		case CursorPositionMiddle:
			cursorIcon = mouse_pressed ?
				QCursor(Qt::ClosedHandCursor) :
				QCursor(Qt::OpenHandCursor);
			break;
		case CursorPositionUndefined:
		default:
			cursorIcon = QCursor(Qt::ArrowCursor);
			break;
	}
	//
	this->setCursor(cursorIcon);
}

const QRectF XLabel::calculateGeometry(const QRectF & _sourceGeometry, const CursorPosition _cursorPosition, const QPointF & _mouseDelta)
{
	QRectF resultGeometry;

	resultGeometry =
		calculateGeometryWithCustomProportions(
			_sourceGeometry, _cursorPosition, _mouseDelta);
	
	if ((resultGeometry.left() >= resultGeometry.right()) ||
		(resultGeometry.top() >= resultGeometry.bottom())) {
		resultGeometry = QRect();
	}
	//
	return resultGeometry;
}


const QRectF XLabel::calculateGeometryWithCustomProportions(
	const QRectF& _sourceGeometry,
	const CursorPosition _cursorPosition,
	const QPointF& _mouseDelta
)
{
	QRectF resultGeometry = _sourceGeometry;
	//
	switch (_cursorPosition)
	{
		case CursorPositionTopLeft:
			resultGeometry.setLeft(_sourceGeometry.left() + _mouseDelta.x());
			resultGeometry.setTop(_sourceGeometry.top() + _mouseDelta.y());
			break;
		case CursorPositionTopRight:
			resultGeometry.setTop(_sourceGeometry.top() + _mouseDelta.y());
			resultGeometry.setRight(_sourceGeometry.right() + _mouseDelta.x());
			break;
		case CursorPositionBottomLeft:
			resultGeometry.setBottom(_sourceGeometry.bottom() + _mouseDelta.y());
			resultGeometry.setLeft(_sourceGeometry.left() + _mouseDelta.x());
			break;
		case CursorPositionBottomRight:
			resultGeometry.setBottom(_sourceGeometry.bottom() + _mouseDelta.y());
			resultGeometry.setRight(_sourceGeometry.right() + _mouseDelta.x());
			break;
		case CursorPositionTop:
			resultGeometry.setTop(_sourceGeometry.top() + _mouseDelta.y());
			break;
		case CursorPositionBottom:
			resultGeometry.setBottom(_sourceGeometry.bottom() + _mouseDelta.y());
			break;
		case CursorPositionLeft:
			resultGeometry.setLeft(_sourceGeometry.left() + _mouseDelta.x());
			break;
		case CursorPositionRight:
			resultGeometry.setRight(_sourceGeometry.right() + _mouseDelta.x());
			break;
		default:
			break;
	}
	//
	return resultGeometry;
}

void XLabel::setImageMap(QImage image_)
{
	image = image_;
	//this->setPixmap(QPixmap::fromImage(image));
	update();
}

void XLabel::setScale(float s)
{
	scale = s;
	if (scale > 4) scale = 4.f;
	if (scale < 0.5) scale = 0.5;
	if (image.isNull()) return;
	// draw the box

	update();
}

QRectF XLabel::getSelectedRect()
{
	return croppingRect;
}
