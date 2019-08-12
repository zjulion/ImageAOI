#ifndef XLABEL_H
#define XLABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>
#include <QImage>


enum CursorPosition
{
	CursorPositionUndefined,
	CursorPositionMiddle,
	CursorPositionTop,
	CursorPositionBottom,
	CursorPositionLeft,
	CursorPositionRight,
	CursorPositionTopLeft,
	CursorPositionTopRight,
	CursorPositionBottomLeft,
	CursorPositionBottomRight
};

class XLabel : public QLabel
{
    Q_OBJECT  

public:
    explicit XLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~XLabel();

    void setImageMap(QImage image);

	void setScale(float s);

	// Ge Selection AOI Rect
	QRectF getSelectedRect();

//signals:
//    void clicked(int x=0, int y = 0);

protected:
	virtual void mousePressEvent(QMouseEvent* event);

	virtual void mouseMoveEvent(QMouseEvent* event);

	virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void wheelEvent(QWheelEvent* e);

	// double click
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

#if 1
	void paintEvent(QPaintEvent* _event) override;
#endif

private:
	QImage image;
	float scale;

	// area selection
	bool selection_mode;
	QRectF croppingRect;

	bool mouse_pressed;
	QPoint start_mouse_pos;
	QRectF last_static_rect;
	CursorPosition cursor_pos;
	 
	CursorPosition cursorPosition(const QRectF& _cropRect, const QPointF& _mousePosition); 

	void updateCursorIcon(const QPointF& _mousePosition);

	const QRectF calculateGeometry(
		const QRectF& _sourceGeometry,
		const CursorPosition _cursorPosition,
		const QPointF& _mouseDelta
	);

	const QRectF calculateGeometryWithCustomProportions(
		const QRectF& _sourceGeometry,
		const CursorPosition _cursorPosition,
		const QPointF& _mouseDelta
	);

	inline QPoint originPoint(const QPoint& pt)
	{
		return QPoint(int(0.5 + pt.x() / scale), int(0.5 + pt.y() / scale));
	}
};

#endif // XLABEL_H