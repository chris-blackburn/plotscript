#include "notebook_app.hpp"

#include <QLayout>

NotebookApp::NotebookApp(QWidget* parent): QWidget(parent) {
	input = new InputWidget(this);
	input->setObjectName("input");

	// TODO: Use qdebug for output, then implement output widget
	// output= new OutputWidget(this);
	// output->setObjectName("output");

	// Setup the layout
	auto layout = new QVBoxLayout;
	layout->addWidget(input);

	setLayout(layout);
	setWindowTitle(tr("Plotscript Notebook"));
}
