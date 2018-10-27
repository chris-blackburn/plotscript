#include <QTest>
#include <QString>
#include <QGraphicsItem>
#include <QList>
#include <QPair>

#include "notebook_app.hpp"

class NotebookTest : public QObject {
	Q_OBJECT

private slots:
	void findInputWidget();
	void findOutputWidget();
	void findOutputView();
	void findOutputScene();
	void simpleExpressions();

	// Tests for point objects
	void testPoint();
	// void testPointLists();

	// TODO: implement additional tests here
private:
	NotebookApp app;

	// Helper functions for repeated tests
	void submitInput(QWidget* w);
	QGraphicsScene* getScene(QWidget* output);
	void verifyNumberOfOutputGraphics(QWidget* output, qreal n);
	void testSimpleTextExpression(const QString& input, const QString& output);
	void testSinglePoint(const QString& input, const QRectF& expected);
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

void NotebookTest::testSimpleTextExpression(const QString& input, const QString& output) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	// Make sure the one graphic is a text object at the center of the scene
	auto scene = getScene(op);
	auto item = scene->itemAt(0, 0, QTransform());

	QGraphicsTextItem* graphic = qgraphicsitem_cast<QGraphicsTextItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not a text item");

	// I use contains here so I can test for errors (i.e. test if starts with "Error:")
	QString msg = "Testing simple text expression: unexpected output for " + input +
		". got \"" + graphic->toPlainText() + "\" expected \"" + output + "\"";
	QVERIFY2(graphic->toPlainText().contains(output), qPrintable(msg));
}

void NotebookTest::testSinglePoint(const QString& input, const QRectF& expected) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	auto scene = getScene(op);
	auto item = scene->items()[0];

	QGraphicsEllipseItem* graphic = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not an ellipse item");

	QCOMPARE(graphic->rect(), expected);
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

void NotebookTest::simpleExpressions() {

	// List of inputs and outputs for simple expressions
	QList<QPair<QString, QString> > tests({
		qMakePair(QString("(1)"), QString("(1)")),
		qMakePair(QString("(\"test\")"), QString("(\"test\")")),
		qMakePair(QString("(+ 1 2)"), QString("(3)")),
		qMakePair(QString("(cos pi)"), QString("(-1)")),
		qMakePair(QString("(+ 1 (* 3 I))"), QString("(1,3)")),
		qMakePair(QString("(first (list (list 1) 2 3))"), QString("(1)")),
		qMakePair(QString("(begin (define inc (lambda (x) (+ 1 x))) (inc 1))"), QString("(2)")),
		qMakePair(QString("("), QString("Error")),
		qMakePair(QString("(+)"), QString("Error")),
		qMakePair(QString("(first (2))"), QString("Error"))
	});

	for (auto test = tests.cbegin(); test != tests.cend(); test++) {
		testSimpleTextExpression(test->first, test->second);
	}
}

void NotebookTest::testPoint() {
	testSinglePoint(QString("(make-point 0 0)"), QRectF(0, 0, 0, 0));
	testSinglePoint(QString("(make-point 10 15)"), QRectF(10, 15, 0, 0));
	testSinglePoint(QString("(set-property \"size\" 20 (make-point 10 10))"),
		QRectF(0, 0, 20, 20));

	// Error handling of point objects
	testSimpleTextExpression(QString("(set-property \"size\" -1 (make-point 0 0))"),
		QString("Error"));
	testSimpleTextExpression(QString("(make-point I 0)"),
		QString("Error"));
	testSimpleTextExpression(QString("(make-point 1 2 3)"),
		QString("Error"));
	testSimpleTextExpression(QString("(set-property \"object-name\" \"point\" 3"),
		QString("Error"));
}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
