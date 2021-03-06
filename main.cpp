#include <QtGui/QApplication>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "utils/audiorecorder.h"
#include "utils/qgspeechapi.h"
#include "utils/qmlclipboardadapter.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    AudioRecorder recorder;
    QGSpeechApi googleApi;

    //load translations
    QTranslator translator;
    translator.load("tr_"+QLocale::system().name(), ":/");
    app->installTranslator(&translator);

    qmlRegisterType<QmlClipboardAdapter>("clipadapter", 1, 0, "QClipboard");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/VoiceInput/main.qml"));
    QObject *root = (QObject *)viewer.rootObject();
    QObject *page = root->findChild<QObject *>("mainPage");

    //connect signals
    QObject::connect(page, SIGNAL(startRecording()), &recorder, SLOT(startRecording()));
    QObject::connect(page, SIGNAL(stopRecording()), &recorder, SLOT(stopRecording()));
    QObject::connect(page, SIGNAL(requestPath()), &recorder, SLOT(requestFilePath()));
    QObject::connect(&recorder, SIGNAL(sendFilePath(QVariant)), page, SLOT(getFilePath(QVariant)));
    QObject::connect(page, SIGNAL(sendToGoogle(QString)), &googleApi, SLOT(sendRequest(QString)));
    QObject::connect(&googleApi, SIGNAL(result(QVariant)), page, SLOT(response(QVariant)));
    QObject::connect(&recorder, SIGNAL(started()), page, SLOT(recordingStarted()));
    QObject::connect(&recorder, SIGNAL(stopped()), page, SLOT(recordingStopped()));

    viewer.showExpanded();

    return app->exec();
}
