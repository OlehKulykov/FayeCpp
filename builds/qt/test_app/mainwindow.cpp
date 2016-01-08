
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace FayeCpp;

static const char * _clientURL = "https://localhost:6001/faye";
static const char * _clientChannel1 = "/xxxxxxxx/xxxxxxxxxxxxx";
static const char * _clientChannel2 = "/xxxxxx";

// SSL datasource
FayeCpp::REString MainWindow::clientLocalCertificateFilePath() const
{
	return FayeCpp::REString("");
}

FayeCpp::REString MainWindow::clientPrivateKeyFilePath() const
{
	return FayeCpp::REString("");
}

FayeCpp::REString MainWindow::clientPrivateKeyPassPhrase() const
{
	return FayeCpp::REString("");
}


FayeCpp::REString MainWindow::clientCACertificateFilePath() const
{
	return FayeCpp::REString("");
}

// delegate
void MainWindow::onFayeTransportConnected(FayeCpp::Client * client)
{
	FayeCpp::RELog::log("DELEGATE onFayeTransportConnected");
}

void MainWindow::onFayeTransportDisconnected(FayeCpp::Client * client)
{
	FayeCpp::RELog::log("DELEGATE onFayeTransportDisconnected");

	client->connect();
    client->subscribeToChannel(_clientChannel1);
    client->subscribeToChannel(_clientChannel2);
}

void MainWindow::onFayeClientConnected(FayeCpp::Client * client)
{
	FayeCpp::RELog::log("DELEGATE onFayeClientConnected");
}

void MainWindow::onFayeClientDisconnected(FayeCpp::Client * client)
{
	FayeCpp::RELog::log("DELEGATE onFayeClientDisconnected");
}

void MainWindow::onFayeClientSubscribedToChannel(FayeCpp::Client * client,
												 const FayeCpp::REString & channel)
{
	FayeCpp::RELog::log("DELEGATE onFayeClientSubscribedToChannel");
}

void MainWindow::onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client,
													 const FayeCpp::REString & channel)
{
	FayeCpp::RELog::log("DELEGATE onFayeClientUnsubscribedFromChannel");
}

void MainWindow::onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client,
														const FayeCpp::REVariantMap & message,
														const FayeCpp::REString & channel)
{
	FayeCpp::RELog::log("DELEGATE onFayeClientReceivedMessageFromChannel");
}

void MainWindow::onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::REVariantMap & message)
{
	FayeCpp::RELog::log("DELEGATE onFayeClientWillSendMessage");
}

void MainWindow::onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
{
	FayeCpp::RELog::log("DELEGATE ERROR: %s", errorString.UTF8String());
}

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow),
_client(NULL)
{
	ui->setupUi(this);

	RELog::log(FayeCpp::Client::info());
#if defined(QT_VERSION)
#endif
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	if (_client)
	{
		delete _client;
		_client = NULL;
	}

	_client = new FayeCpp::Client();

	_client->setUsingIPV6(false);
    _client->setUrl(_clientURL);
	_client->setDelegate(this);
	//	_client->setSSLDataSource(new FayeSSLDataSource());
}

void MainWindow::on_pushButton_2_clicked()
{
	if (_client)
	{
		_client->connect();
        _client->subscribeToChannel(_clientChannel1);
        _client->subscribeToChannel(_clientChannel2);
	}
}

void MainWindow::on_pushButton_3_clicked()
{
	if (_client)
	{
		REVariantMap message;

		RELog::log("Try send text: %s", ui->lineEdit->text().toStdString().c_str());

		message["text"] = ui->lineEdit->text().toStdString().c_str();

        _client->sendMessageToChannel(message, _clientChannel1);
	}
}
