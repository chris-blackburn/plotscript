#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget* parent): QWidget(parent) {
	scene = new QGraphicsScene(this);
	view = new QGraphicsView(scene);

	// This widget also needs a layout to put the view inside of
	auto layout = new QVBoxLayout;
	layout->addWidget(view);

	setLayout(layout);
}

void OutputWidget::addText(const QString& str) {
	scene->addText(str);
}

void OutputWidget::processExpression(const Expression& exp) {
	addText(QString::fromStdString(exp.head().asSymbol()));
}

void OutputWidget::error(const QString& str) {
	addText(str);
}

void OutputWidget::clear() {
	scene->clear();
}
