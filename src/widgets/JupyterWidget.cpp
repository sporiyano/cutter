
#ifdef CUTTER_ENABLE_JUPYTER

#include "ui_JupyterWidget.h"

#include "JupyterWidget.h"

#include <QTabWidget>
#include <QHBoxLayout>
#include <QLabel>

#ifdef CUTTER_ENABLE_QTWEBENGINE
#include <QWebEngineSettings>
#endif

JupyterWidget::JupyterWidget(QWidget *parent, Qt::WindowFlags flags) :
    QDockWidget(parent, flags),
    ui(new Ui::JupyterWidget)
{
    ui->setupUi(this);

    connect(Jupyter(), &JupyterConnection::urlReceived, this, &JupyterWidget::urlReceived);
    connect(Jupyter(), &JupyterConnection::creationFailed, this, &JupyterWidget::creationFailed);
    Jupyter()->start();
}

JupyterWidget::~JupyterWidget()
{
}

#ifdef CUTTER_ENABLE_QTWEBENGINE
JupyterWebView *JupyterWidget::createNewTab()
{
    auto webView = new JupyterWebView(this);
    ui->tabWidget->addTab(webView, "Tab");
    webView->setTabWidget(ui->tabWidget);
    return webView;
}
#endif

void JupyterWidget::urlReceived(const QString &url)
{
#ifdef CUTTER_ENABLE_QTWEBENGINE
    createNewTab()->load(QUrl(url));
#else
    QWidget *failPage = new QWidget(this);
    QLabel *label = new QLabel(failPage);
    label->setText(tr("Cutter has been built without QtWebEngine.<br />Open the following URL in your Browser to use Jupyter:<br /><a href=\"%1\">%1</a>").arg(url));
    label->setTextFormat(Qt::RichText);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label->setOpenExternalLinks(true);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    layout->setAlignment(label, Qt::AlignCenter);
    failPage->setLayout(layout);
    ui->tabWidget->addTab(failPage, tr("Jupyter"));
#endif
}

void JupyterWidget::creationFailed()
{
    QWidget *failPage = new QWidget(this);
    QLabel *label = new QLabel(failPage);
    label->setText(tr("An error occurred while opening jupyter. Make sure Jupyter is installed system-wide."));
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    layout->setAlignment(label, Qt::AlignCenter);
    failPage->setLayout(layout);
    ui->tabWidget->addTab(failPage, tr("Error"));
}


#ifdef CUTTER_ENABLE_QTWEBENGINE
JupyterWebView::JupyterWebView(JupyterWidget *mainWidget, QWidget *parent) : QWebEngineView(parent)
{
    this->mainWidget = mainWidget;
    this->tabWidget = nullptr;

    connect(this, &QWebEngineView::titleChanged, this, &JupyterWebView::onTitleChanged);
}

void JupyterWebView::setTabWidget(QTabWidget *tabWidget)
{
    this->tabWidget = tabWidget;
    updateTitle();
}

QWebEngineView *JupyterWebView::createWindow(QWebEnginePage::WebWindowType type)
{
    switch (type)
    {
        case QWebEnginePage::WebBrowserTab:
            return mainWidget->createNewTab();
        default:
            return nullptr;
    }
}

void JupyterWebView::onTitleChanged(const QString &)
{
    updateTitle();
}

void JupyterWebView::updateTitle()
{
    if (!tabWidget)
    {
        return;
    }

    QString title = this->title();
    if (title.isEmpty())
    {
        title = tr("Jupyter");
    }
    tabWidget->setTabText(tabWidget->indexOf(this), title);
}

#endif

#endif
