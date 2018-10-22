#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget* parent): QWidget(parent) {}

#include <QDebug>
void OutputWidget::update(const QString& str) {
	qDebug() << str;
}

void OutputWidget::error(const QString& str) {
	qDebug() << str;
}
