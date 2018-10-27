#include <QTest>
#include <QGraphicsItem>

#include "notebook_app.hpp"

class NotebookTest : public QObject {
	Q_OBJECT

private slots:
	void findInputWidget();
	void findOutputWidget();
	void findOutputView();
	void findOutputScene();
	void testSimpleExpressions();

	// TODO: implement additional tests here
private:
	NotebookApp app;

	// Helper functions for repeated tests
	void submitInput(QWidget* w);
	QGraphicsScene* getScene(QWidget* output);
	void verifyNumberOfOutputGraphics(QWidget* output, qreal n);
};

// ************ START: Helper functions for repeated tests ************
void NotebookTest::submitInput(QWidget* w) {
	QTest::keyPress(w, Qt::Key_Return, Qt::ShiftModifier);
	QTest::keyRelease(w, Qt::Key_Return, Qt::ShiftModifier);
}

QGraphicsScene* NotebookTest::getScene(QWidget* output) {
	auto view = output->findChild<QGraphicsView*>();
	return view->scene();
}

void NotebookTest::verifyNumberOfOutputGraphics(QWidget* output, qreal n) {
	auto children = getScene(output)->items();
	QVERIFY2(children.size() == n, "Wrong number of graphics items in the output");
}


// ************ END: Helper functions for repeated tests ************
void NotebookTest::findInputWidget() {
	auto ip = app.findChild<QWidget*>("input");

	QVERIFY2(ip, "Could not find widget with name: 'input'");
}

void NotebookTest::findOutputWidget() {
	auto op = app.findChild<QWidget*>("output");

	QVERIFY2(op, "Could not find widget with name: 'output'");
}

void NotebookTest::findOutputView() {
	auto op = app.findChild<QWidget*>("output");
	auto view = op->findChild<QGraphicsView*>();

	QVERIFY2(view, "Could not find a view in the output widget");
}

void NotebookTest::findOutputScene() {
	auto op = app.findChild<QWidget*>("output");
	auto view = op->findChild<QGraphicsView*>();
	auto scene = view->scene();

	QVERIFY2(scene, "Could not find a scene in the output widget");
}

void NotebookTest::testSimpleExpressions() {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, "(1)");
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	// Make sure the one graphic is a text object at the center of the scene
	auto scene = getScene(op);
	auto item = scene->itemAt(0, 0, QTransform());

	QGraphicsTextItem* graphic = qgraphicsitem_cast<QGraphicsTextItem*>(item);
	QVERIFY2(graphic != NULL,
		"Graphics item is not a graphics item");

	QVERIFY(graphic->toPlainText() == "(1)");
}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
