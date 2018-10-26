#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget* parent): QWidget(parent) {
	scene = new QGraphicsScene(this);
	view = new QGraphicsView(scene);

	// This widget also needs a layout to put the view inside of
	auto layout = new QVBoxLayout;
	layout->addWidget(view);

	setLayout(layout);
}

void OutputWidget::addText(const std::string& str) {
	scene->addText(QString::fromStdString(str));
}

void OutputWidget::addText(const QString& str) {
	scene->addText(str);
}

void OutputWidget::handleObject(const Expression& exp, const std::string& objectName) {
	exp.head();
	addText(objectName);
	return;
}

void OutputWidget::processExpression(const Expression& exp) {

	// lambda functions don't get printed out
	if (!exp.isHeadLambdaRoot()) {
		if (exp.head().isNone()) {
			addText((QString)"NONE");
		} else if (exp.isHeadListRoot()) {

			// As of now, items in lists just get printed out on top of each other. We just recurse
			// through and add everything to the scene
			for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
				processExpression(*e);
			}
		} else {

			// First, check if the expression has an object-name property that matches one of
			// the graphic primitive types
			Expression objectName = exp.getProperty("object-name");
			if (!objectName.head().isNone()) {
				handleObject(exp, objectName.head().asSymbol(true));
			} else {

				// Everything else prints the same as the REPL, the head is just wrapped in
				// a set of parentheses
				std::stringstream out;
				out << "(" << exp.head() << ")";
				addText(out.str());
			}
		}
	}
}

void OutputWidget::error(const QString& str) {
	addText(str);
}

void OutputWidget::clear() {
	scene->clear();
}
