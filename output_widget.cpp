#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget* parent): QWidget(parent) {
	scene = new QGraphicsScene(this);
	view = new QGraphicsView(scene);

	// This widget also needs a layout to put the view inside of
	auto layout = new QVBoxLayout;
	layout->addWidget(view);

	setLayout(layout);
}

void OutputWidget::processExpression(const Expression& exp) {

	// lambda functions don't get printed out
	if (!exp.isHeadLambdaRoot()) {
		if (exp.head().isNone()) {
			scene->addText("NONE");
		} else if (exp.isHeadListRoot()) {

			// As of now, items in lists just get printed out on top of each other. We just recurse
			// through and add everything to the scene
			for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
				processExpression(*e);
			}
		} else {

			// Everything else prints the same as the REPL, the head is just wrapped in
			// a set of parentheses
			std::stringstream out;
			out << "(" << exp.head() << ")";
			scene->addText(QString::fromStdString(out.str()));
		}
	}
}

void OutputWidget::error(const QString& str) {
	scene->addText(str);
}

void OutputWidget::clear() {
	scene->clear();
}
