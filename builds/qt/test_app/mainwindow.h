#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "fayecpp.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow, FayeCpp::Delegate, FayeCpp::SSLDataSource
{
	Q_OBJECT

private:
	FayeCpp::Client * _client;


public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

	// SSL datasource
	virtual FayeCpp::REString clientLocalCertificateFilePath() const;
	virtual FayeCpp::REString clientPrivateKeyFilePath() const;
	virtual FayeCpp::REString clientPrivateKeyPassPhrase() const;
	virtual FayeCpp::REString clientCACertificateFilePath() const;

	// delegate
	virtual void onFayeTransportConnected(FayeCpp::Client * client);
	virtual void onFayeTransportDisconnected(FayeCpp::Client * client);
	virtual void onFayeClientConnected(FayeCpp::Client * client);
	virtual void onFayeClientDisconnected(FayeCpp::Client * client);
	virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client,
												 const FayeCpp::REString & channel);
	virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client,
													 const FayeCpp::REString & channel);
	virtual void onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client,
														const FayeCpp::REVariantMap & message,
														const FayeCpp::REString & channel);
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::REVariantMap & message);
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString);

private slots:
	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

	void on_pushButton_3_clicked();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
