#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMessageBox>

class FusionClient : public QWidget {
    Q_OBJECT
public:
    FusionClient(QWidget* parent = nullptr) : QWidget(parent), socket(new QTcpSocket(this)) {
        setWindowTitle("Fusion++ Multiplayer GUI Client");
        setMinimumSize(600, 400);

        hostEdit = new QLineEdit("127.0.0.1");
        portEdit = new QLineEdit("8765");
        connectBtn = new QPushButton("Connect");
        statusLabel = new QLabel("Disconnected");

        QHBoxLayout* connLayout = new QHBoxLayout;
        connLayout->addWidget(new QLabel("Host:"));
        connLayout->addWidget(hostEdit);
        connLayout->addWidget(new QLabel("Port:"));
        connLayout->addWidget(portEdit);
        connLayout->addWidget(connectBtn);
        connLayout->addWidget(statusLabel);

        cmdEdit = new QLineEdit;
        sendBtn = new QPushButton("Send Command");
        QHBoxLayout* sendLayout = new QHBoxLayout;
        sendLayout->addWidget(cmdEdit);
        sendLayout->addWidget(sendBtn);

        output = new QTextEdit;
        output->setReadOnly(true);

        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->addLayout(connLayout);
        mainLayout->addLayout(sendLayout);
        mainLayout->addWidget(new QLabel("Output Log:"));
        mainLayout->addWidget(output);
        setLayout(mainLayout);

        connect(connectBtn, &QPushButton::clicked, this, &FusionClient::onConnect);
        connect(sendBtn, &QPushButton::clicked, this, &FusionClient::onSend);
        connect(socket, &QTcpSocket::connected, this, &FusionClient::onConnected);
        connect(socket, &QTcpSocket::disconnected, this, &FusionClient::onDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &FusionClient::onReadyRead);
        connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                this, &FusionClient::onError);
    }

private slots:
    void onConnect() {
        socket->abort();
        socket->connectToHost(hostEdit->text(), portEdit->text().toUShort());
        output->append("Connecting to " + hostEdit->text() + ":" + portEdit->text() + "...");
    }

    void onConnected() {
        statusLabel->setText("Connected");
        output->append("[Connected]");
    }

    void onDisconnected() {
        statusLabel->setText("Disconnected");
        output->append("[Disconnected]");
    }

    void onError(QAbstractSocket::SocketError) {
        statusLabel->setText("Error");
        output->append("[❌] " + socket->errorString());
    }

    void onSend() {
        if (socket->state() != QTcpSocket::ConnectedState) {
            QMessageBox::warning(this, "Not Connected", "Please connect to the server first.");
            return;
        }
        QString msg = cmdEdit->text().trimmed();
        if (!msg.isEmpty()) {
            socket->write(msg.toUtf8());
            output->append("> " + msg);
            cmdEdit->clear();
        }
    }

    void onReadyRead() {
        QByteArray data = socket->readAll();
        if (!data.isEmpty())
            output->append(QString::fromUtf8(data));
    }

private:
    QLineEdit* hostEdit;
    QLineEdit* portEdit;
    QPushButton* connectBtn;
    QLabel* statusLabel;
    QLineEdit* cmdEdit;
    QPushButton* sendBtn;
    QTextEdit* output;
    QTcpSocket* socket;
};

#include "fusionpp_multiplayer_gui.moc"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    FusionClient w;
    w.show();
    return app.exec();
}

#include <QApplication>
#include <QTabWidget>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QMap>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class EventGraphWidget : public QWidget {
    Q_OBJECT
public:
    EventGraphWidget(QWidget* parent = nullptr) : QWidget(parent) {}

    void addEvent(const QString& evt) {
        eventCounts[evt]++;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.fillRect(rect(), Qt::white);

        int barW = width() / qMax(1, eventCounts.size());
        int maxCount = 1;
        for (auto c : eventCounts.values()) maxCount = qMax(maxCount, c);

        int x = 0;
        for (auto it = eventCounts.begin(); it != eventCounts.end(); ++it) {
            int barH = int((float(it.value()) / maxCount) * (height() - 30));
            p.setBrush(Qt::blue);
            p.drawRect(x, height()-barH-20, barW-10, barH);
            p.setPen(Qt::black);
            p.drawText(x, height()-5, it.key());
            x += barW;
        }
    }

private:
    QMap<QString, int> eventCounts;
};

class FusionClient : public QWidget {
    Q_OBJECT
public:
    FusionClient(QWidget* parent = nullptr) : QWidget(parent), socket(new QTcpSocket(this)) {
        setWindowTitle("Fusion++ Multiplayer GUI Client");
        setMinimumSize(800, 600);

        // --- Tabs ---
        tabs = new QTabWidget(this);

        // === Multiplayer Command Tab ===
        QWidget* cmdTab = new QWidget;
        hostEdit = new QLineEdit("127.0.0.1");
        portEdit = new QLineEdit("8765");
        connectBtn = new QPushButton("Connect");
        statusLabel = new QLabel("Disconnected");

        QHBoxLayout* connLayout = new QHBoxLayout;
        connLayout->addWidget(new QLabel("Host:"));
        connLayout->addWidget(hostEdit);
        connLayout->addWidget(new QLabel("Port:"));
        connLayout->addWidget(portEdit);
        connLayout->addWidget(connectBtn);
        connLayout->addWidget(statusLabel);

        cmdEdit = new QLineEdit;
        sendBtn = new QPushButton("Send Command");
        QHBoxLayout* sendLayout = new QHBoxLayout;
        sendLayout->addWidget(cmdEdit);
        sendLayout->addWidget(sendBtn);

        output = new QTextEdit;
        output->setReadOnly(true);

        QVBoxLayout* cmdLayout = new QVBoxLayout;
        cmdLayout->addLayout(connLayout);
        cmdLayout->addLayout(sendLayout);
        cmdLayout->addWidget(new QLabel("Output Log:"));
        cmdLayout->addWidget(output);
        cmdTab->setLayout(cmdLayout);
        tabs->addTab(cmdTab, "Multiplayer Command");

        // === .fpp Editor Tab ===
        QWidget* editorTab = new QWidget;
        fppEdit = new QTextEdit;
        QPushButton* openBtn = new QPushButton("Open");
        QPushButton* saveBtn = new QPushButton("Save");
        QPushButton* sendFppBtn = new QPushButton("Send to Engine");
        QHBoxLayout* edBtnLayout = new QHBoxLayout;
        edBtnLayout->addWidget(openBtn);
        edBtnLayout->addWidget(saveBtn);
        edBtnLayout->addWidget(sendFppBtn);

        QVBoxLayout* editorLayout = new QVBoxLayout;
        editorLayout->addLayout(edBtnLayout);
        editorLayout->addWidget(fppEdit);
        editorTab->setLayout(editorLayout);
        tabs->addTab(editorTab, "Live .fpp Editor");

        // === Realtime Log/Graph Tab ===
        QWidget* logTab = new QWidget;
        logView = new QTextEdit;
        logView->setReadOnly(true);
        eventGraph = new EventGraphWidget;
        QVBoxLayout* logLayout = new QVBoxLayout;
        logLayout->addWidget(new QLabel("Realtime Log:"));
        logLayout->addWidget(logView);
        logLayout->addWidget(new QLabel("Event Graph:"));
        logLayout->addWidget(eventGraph, 1);
        logTab->setLayout(logLayout);
        tabs->addTab(logTab, "Log & Events");

        // === Layout ===
        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->addWidget(tabs);
        setLayout(mainLayout);

        // --- Networking ---
        connect(connectBtn, &QPushButton::clicked, this, &FusionClient::onConnect);
        connect(sendBtn, &QPushButton::clicked, this, &FusionClient::onSend);
        connect(socket, &QTcpSocket::connected, this, &FusionClient::onConnected);
        connect(socket, &QTcpSocket::disconnected, this, &FusionClient::onDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &FusionClient::onReadyRead);
        connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                this, &FusionClient::onError);

        // --- .fpp Editor Actions ---
        connect(openBtn, &QPushButton::clicked, this, &FusionClient::onOpenFpp);
        connect(saveBtn, &QPushButton::clicked, this, &FusionClient::onSaveFpp);
        connect(sendFppBtn, &QPushButton::clicked, this, &FusionClient::onSendFpp);

        // --- Periodic Log Poll ---
        QTimer* logTimer = new QTimer(this);
        connect(logTimer, &QTimer::timeout, this, &FusionClient::refreshLog);
        logTimer->start(2000);
    }

private slots:
    void onConnect() {
        socket->abort();
        socket->connectToHost(hostEdit->text(), portEdit->text().toUShort());
        output->append("Connecting to " + hostEdit->text() + ":" + portEdit->text() + "...");
    }
    void onConnected() {
        statusLabel->setText("Connected");
        output->append("[Connected]");
    }
    void onDisconnected() {
        statusLabel->setText("Disconnected");
        output->append("[Disconnected]");
    }
    void onError(QAbstractSocket::SocketError) {
        statusLabel->setText("Error");
        output->append("[❌] " + socket->errorString());
    }
    void onSend() {
        if (socket->state() != QTcpSocket::ConnectedState) {
            QMessageBox::warning(this, "Not Connected", "Please connect to the server first.");
            return;
        }
        QString msg = cmdEdit->text().trimmed();
        if (!msg.isEmpty()) {
            socket->write(msg.toUtf8());
            output->append("> " + msg);
            cmdEdit->clear();
        }
    }
    void onReadyRead() {
        QByteArray data = socket->readAll();
        if (!data.isEmpty()) {
            QString msg = QString::fromUtf8(data);
            output->append(msg);
            logView->append(QDateTime::currentDateTime().toString("[hh:mm:ss] ") + msg.trimmed());
            if (!msg.trimmed().isEmpty())
                eventGraph->addEvent(msg.split(' ').first());
        }
    }
    void onOpenFpp() {
        QString path = QFileDialog::getOpenFileName(this, "Open .fpp file", "", "*.fpp");
        if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                fppEdit->setPlainText(in.readAll());
            }
        }
    }
    void onSaveFpp() {
        QString path = QFileDialog::getSaveFileName(this, "Save .fpp file", "", "*.fpp");
        if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << fppEdit->toPlainText();
            }
        }
    }
    void onSendFpp() {
        if (socket->state() != QTcpSocket::ConnectedState) {
            QMessageBox::warning(this, "Not Connected", "Please connect to the server first.");
            return;
        }
        // Save to local .fpp file, then send autorun command
        QString tempPath = "temp_gui_autogen.fpp";
        QFile file(tempPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << fppEdit->toPlainText();
            file.close();
        }
        QString cmd = "autorun_fpp " + tempPath;
        socket->write(cmd.toUtf8());
        output->append("> [Autorun] " + cmd);
    }
    void refreshLog() {
        // Optional: reload external log file or pull from HTTP dashboard
        // Here, you could parse reinforce_log.json, but demo just scrolls with onReadyRead
    }

private:
    QTabWidget* tabs;
    // Multiplayer tab
    QLineEdit* hostEdit;
    QLineEdit* portEdit;
    QPushButton* connectBtn;
    QLabel* statusLabel;
    QLineEdit* cmdEdit;
    QPushButton* sendBtn;
    QTextEdit* output;
    QTcpSocket* socket;
    // .fpp Editor tab
    QTextEdit* fppEdit;
    // Log/Graph tab
    QTextEdit* logView;
    EventGraphWidget* eventGraph;
};

#include "fusionpp_multiplayer_gui.moc"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    FusionClient w;
    w.show();
    return app.exec();
}

