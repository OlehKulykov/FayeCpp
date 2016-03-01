

import UIKit

class ViewController: UIViewController, FayeCppClientDelegate {

	var client: FayeCppClient = FayeCppClient();
	let channel1: String = "/xxxxxxxxx/xxxxxxxxxxxx";
	let channel2: String = "/xxxxxxxxx/xxxxxxxxxxxx";

	func onFayeTransportConnected(client: FayeCppClient) {
		print("Faye transport connected")
	}

	func onFayeClientConnected(client: FayeCppClient) {
		print("Faye client connected")

		client.sendMessage(["key": "value"], toChannel: channel1)
	}

	func onFayeClient(client: FayeCppClient, receivedMessage message: [NSObject : AnyObject]?, fromChannel channel: String?) {
		print("Received from channel \(channel) message: \(message) ")
	}


	override func viewDidLoad() {
		super.viewDidLoad();

		client.delegate = self

		client.usingIPV6 = true
		client.urlString = "xxxx://xxxxxxxxxxx:80/faye"
		client.connect()
		client.subscribeToChannel(channel1)
		client.subscribeToChannel(channel2)
	}

	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()

		// Dispose of any resources that can be recreated.
	}


}

