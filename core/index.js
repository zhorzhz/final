const dgram = require('dgram');

// Create a UDP socket
const server = dgram.createSocket('udp4');

// Define the port to listen on
const PORT = 3330;

// Define the Arduino's IP address and UDP port
const remoteControllerHost = 'nodemobile'; // Replace with the actual IP address
const remoteControllerPort = 3330; // Arduino's UDP port

// Create a UDP socket
const client = dgram.createSocket('udp4');

// Handle any errors that occur
client.on('error', (error) => {
    console.error(`UDP socket error: ${error.message}`);
});

let detectedObjects = {

}

let alarmEnabledObjects = {
    "person": true,
    "cat": true,
    "movement": true,
    "open door": true,
    "open window": true,
}

function detectedObject(detectedObjectName) {
    if (detectedObjects[detectedObjectName]) {
        // we already notified the mobile device for this object, now it is in cool down period
        return;
    }
    console.log("new object detected", detectedObjectName)
    detectedObjects[detectedObjectName] = true;
    // reset object detection after certain time
    setTimeout(() => {
        detectedObjects[detectedObjectName] = false;
        console.log("cleared", detectedObjectName);
    }, 5000);


    if (alarmEnabledObjects[detectedObjectName]) {
        notifyRemoteController(detectedObjectName);
    }
}

// Listen for incoming messages
server.on('message', (msg, rinfo) => {
    msg = JSON.parse(msg);
    switch (msg.node) {
        case "nodewindow":
            detectedObject("open window");
            return;
        case "nodedoor":
            detectedObject("open door");
            return;
        case "nodepir":
            detectedObject("movement");
            return;
        case "nodecamera":
            msg.results.forEach((data) => {
                detectedObject(data.name);
            })
            return;
    }
});

function notifyRemoteController(intruder) {
    try {
        // Convert the JSON message to a string
        const jsonString = JSON.stringify({
            alarm: true,
            intruder: intruder,
        });

        // Send the JSON message as a UDP packet
        client.send(jsonString, remoteControllerPort, remoteControllerHost, (error) => {
            if (error) {
                console.error('Error sending UDP packet:', error);
            } else {
                console.log('Sent JSON message to Arduino:', jsonString);
            }
        });
    } catch(err) {
        console.error("Failed to send to remote controller")
    }
}

// Bind the server to the specified port
server.bind(PORT, () => {
    console.log(`Server listening on port ${PORT}`);
});

// Handle errors
server.on('error', (err) => {
    console.error(`Server error:\n${err.stack}`);
    server.close();
});