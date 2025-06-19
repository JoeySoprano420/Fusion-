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
