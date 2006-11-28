
#include "tidle.moc"
#include "tidle.h"

int main(int argc, char *argv[])
{
    int port = 8000;
    QString host = "localhost";

    QApplication a(argc, argv);

    MyDialog *listview = new MyDialog();

    listview->resize(404, 204);
    listview->setCaption("Tasker");
    listview->show();
    a.setMainWidget(listview);

    return a.exec();
}
