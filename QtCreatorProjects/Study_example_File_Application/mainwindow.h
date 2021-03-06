#include <QMainWindow>
#include <QSessionManager>
#include <QPlainTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

    void loadFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void about();
    void documentWasModified();

#ifndef QT_NO_SESSIONMANAGER
    void commitDate(QSessionManager &);
#endif

private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;

    QString curFile;

};


