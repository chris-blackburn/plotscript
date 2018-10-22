#include "notebook_app.hpp"

#include <QLayout>

NotebookApp::NotebookApp(QWidget* parent): QWidget(parent) {
	input = new InputWidget(this);
	input->setObjectName("input");

	output= new OutputWidget(this);
	output->setObjectName("output");

	// Setup the layout
	auto layout = new QVBoxLayout;
	layout->addWidget(input);
	layout->addWidget(output);

	setLayout(layout);
}
