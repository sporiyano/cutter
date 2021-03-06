
#ifndef JUPYTERWIDGET_H
#define JUPYTERWIDGET_H

#ifdef CUTTER_ENABLE_JUPYTER

#include <memory>

#include <QDockWidget>

#include "utils/JupyterConnection.h"

namespace Ui
{
    class JupyterWidget;
}

class JupyterWebView;

class QTabWidget;

class JupyterWidget : public QDockWidget
{
Q_OBJECT

public:
    JupyterWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~JupyterWidget();

#ifdef CUTTER_ENABLE_QTWEBENGINE
    JupyterWebView *createNewTab();
#endif

private slots:
    void urlReceived(const QString &url);
    void creationFailed();

private:
    std::unique_ptr<Ui::JupyterWidget> ui;
};

#ifdef CUTTER_ENABLE_QTWEBENGINE

#include <QWebEngineView>

class JupyterWebView : public QWebEngineView
{
Q_OBJECT

public:
    JupyterWebView(JupyterWidget *mainWidget, QWidget *parent = nullptr);

    void setTabWidget(QTabWidget *tabWidget);

protected:
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

private slots:
    void onTitleChanged(const QString &title);

private:
    JupyterWidget *mainWidget;
    QTabWidget *tabWidget;

    void updateTitle();
};

#endif

#endif

#endif //JUPYTERWIDGET_H
