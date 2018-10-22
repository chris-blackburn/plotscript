#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

class OutputWidget: public QWidget {
Q_OBJECT
public:
	OutputWidget(QWidget* parent = nullptr);
public slots:
	void update(const QString& str);
};

#endif
