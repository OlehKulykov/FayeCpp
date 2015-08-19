//
//  ViewController.swift
//  TestFayeSw
//
//  Created by Resident evil on 8/10/15.
//  Copyright (c) 2015 Resident evil. All rights reserved.
//

import UIKit

class ViewController: UIViewController, FayeCppClientDelegate {

	var client: FayeCppClient = FayeCppClient();
	let channel1: String = "/xxxxxxxxx/xxxxxxxxxxxx";
	let channel2: String = "/xxxxxxxxx/xxxxxxxxxxxx";

	func onFayeTransportConnected(client: FayeCppClient) {
		println("Faye transport connected")
	}

	func onFayeClientConnected(client: FayeCppClient) {
		println("Faye client connected")

		client.sendMessage(["key": "value"], toChannel: channel1)
	}

	func onFayeClient(client: FayeCppClient, receivedMessage message: [NSObject : AnyObject]?, fromChannel channel: String?) {
		println("Received from channel \(channel) message: \(message) ")
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

