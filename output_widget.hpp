#ifndef OUTPUT_WIDGET_HPP 
#define OUTPUT_WIDGET_HPP 

#include <QGraphicsView>
#include <QGraphicsScene>

class OutputWidget: public QWidget {
Q_OBJECT
public:
	OutputWidget(QWidget* parent = nullptr);
};

#endif
