# EPAS-Embeded-Client

- [EPAS-Embeded-Client](#epas-embeded-client)
  - [Project Introduction](#project-introduction)
  - [Operation Method](#operation-method)
  - [Expected Effects](#expected-effects)

## Project Introduction

This repository contains the code for the EPAS client system for embedded systems. EPAS was initially developed as a plugin to be embedded in navigation, but it was also implemented as a separate embedded device for users who do not use (or have difficulty using) navigation. The EPAS client device does not require a large computing power, only GPS and network communication functions. Therefore, it can run sufficiently on low-performance single-threaded embedded systems such as Arduino. This allows existing vehicles to add the EPAS system at a low cost, and if a smartphone or vehicle that has navigation system, the EPAS can be used with only a software update without additional hardware.

Source code can be found at this link: [EPAS-Embeded-Client](https://github.com/Ajou-Soft-19/EPAS-Client/blob/main/embedded/gsc_node_mcu.ino)

|                                          EPAS Implementaion                                            |             EPAS Client Divice Prototype               |
| :------------------------------------------------------------------------------------------------: | :----------------------------------------------: |
| <img src="https://github.com/Ajou-Soft-19/EPAS-Embeded-Client/assets/32717522/db2253ca-8453-4697-8732-e4e98be4027a" alt="EPAS" style="display: block; margin-left: auto; margin-right: auto;" /> | <img src="https://github.com/Ajou-Soft-19/EPAS-Embeded-Client/assets/32717522/06179948-e7e7-4f62-894e-7a36c9a6e1fe" alt="Additional Information" style="display: block; margin-left: auto; margin-right: auto;" /> |

The above picture is a prototype picture of the developed EPAS client device. The hardware used in this device is as follows.

| No.  |          Name        | Quantity | Price (USD) |
|------|----------------------|------|------------|
| 1    | NodeMCU V3           | 1    | $1.8       |
| 2    | GPS Module (Neo-7M)  | 1    | $5.7       |
| 3    | LCD (16x2)           | 1    | $1.0       |
| 4    | DFPlayer Mini        | 1    | $0.8       |
| 5    | Speaker              | 1    | $1.3       |
| 6    | Micro SD (16GB)      | 1    | $3.0       |

**Total Price:  $13.6**

Prototype was made for development and testing purposes, and actual products can be made with smaller and cheaper hardware through wholesale.

## Operation Method

| EPAS Warning Message |
| :-------------------: |
| <img src="https://github.com/Ajou-Soft-19/EPAS-Embeded-Client/assets/32717522/4702b382-53fd-4043-834f-7d6f8b6fdb7e" width="700px"> |

- When an emergency vehicle warning is issued, it displays the **license number of the emergency vehicle**, **the direction of approach of the emergency vehicle**, and **the number of emergency vehicles around**.

EPAS Client Device checks its current location through the GPS module and sends the current location to the [EPAS Vehicle Monitoring Server](https://github.com/Ajou-Soft-19/spring-socket-server) via socket communication. Based on this, the EPAS server checks if an emergency vehicle is passing by and if it is expected to meet an emergency vehicle, it sends a warning message to the client device. The client device receives this and displays a warning message on the LCD and outputs a warning sound through the DFPlayer Mini. At this time, the user can check the license number of the emergency vehicle and the direction in which the emergency vehicle is approaching through the LCD monitor and speaker.

If there are multiple emergency vehicles around, the EPAS client will display warning messages in order and notify the user. Users can be aware of emergency vehicles in advance and respond to them.

## Expected Effects

EPAS client devices can expect the following effects.

1. **Ensuring User Safety**: By recognizing and avoiding the approach of emergency vehicles in advance, it ensures the safety of users.
2. **Accessibility to Existing Vehicles**: Existing vehicles can also use the EPAS system at a low cost, making it highly accessible.
3. **User Convenience**: Users can easily check the direction of approach of emergency vehicles through the LCD and speaker, making it easy to understand the surrounding situation.
4. **Improving Traffic Flow**: If general vehicle drivers can recognize and respond to the approach of emergency vehicles in advance, they can efficiently create a path for emergency vehicles. This can improve traffic flow and contribute to reducing traffic congestion.
5. **Improving Social Awareness**: By promoting and using the EPAS system, the awareness of emergency vehicles among drivers can be improved. By naturally establishing a culture of giving way to emergency vehicles, the overall emergency response capability of society can be enhanced.
6. **Technical Scalability**: The EPAS client device is a cheap solution that can be easily installed in existing vehicles, but various technical expansions are possible in the future. For example, by utilizing artificial intelligence to analyze the user's driving habits, more effective response measures to the approach of emergency vehicles can be proposed.

If the EPAS system is introduced into a social-based system, it can provide great help for the passage of emergency vehicles at a low cost and contribute to the safe driving of users.

# EPAS API DOCUMENTATION

- [EPAS API Documentation](#epas-api-documentation)
  - [Ordinary User Application API](#ordinary-user-application-api)
    - [1. EPAS API](#1-epas-api)
    - [2. EPAS Server Connection](#2-epas-server-connection)
    - [3. Update Location on EPAS Server](#3-update-location-on-epas-server)
    - [4. Receiving Emergency Vehicle Information from the EPAS Server](#4-receiving-emergency-vehicle-information-from-the-epas-server)
    - [5. Disconnecting from the EPAS Server](#5-disconnecting-from-the-epas-server)
  - [Emergency Vehicle Driver Application API](#emergency-vehicle-driver-application-api)
    - [1. Sign Up and Log In](#1-sign-up-and-log-in)
    - [2. Access Token Issuance and Rotation](#2-access-token-issuance-and-rotation)
    - [3. EPAS Socket Server Connection](#3-epas-socket-server-connection)
    - [4. Update Location on EPAS Server](#4-update-location-on-epas-server)
    - [5. Registering and Updating Emergency Situations on the EPAS Server](#5-registering-and-updating-emergency-situations-on-the-epas-server)
    - [6. Ending an Emergency Situation](#6-ending-an-emergency-situation)

## Ordinary User Application API

### 1. EPAS API

EPAS Client does not require any user information. Users can easily receive emergency vehicle data by communicating with the EPAS server via a socket. The EPAS server identifies the user's location based on the received GPS information and sends notifications only when necessary.
Developers can provide users with appropriate notifications through the response JSON provided in the API.

### 2. EPAS Server Connection

To connect to the EPAS server, use the following API address.

**EPAS Connection Address**

```
wss://ajou-epas.xyz:7002/ws/my-location
```

After establishing a socket connection, when you are ready to start socket communication, send an INIT message to the server. The INIT message to be sent is as follows.

```json
{
  "requestType": "INIT"
}
```

The server, upon receiving the INIT message, identifies the user's location and prepares to send an emergency vehicle notification. Once the preparation is complete, the server sends the following response.

```json
{
  "code": 200,
  "messageType": "RESPONSE",
  "data": {
    "vehicleStatusId": "e6ba5180-7612-bc53-e98e-8ae2fee7a7e3"
  }
}
```

The vehicleStatusId is a temporary unique identifier for identifying anonymous users.

### 3. Update Location on EPAS Server

After successfully establishing a connection through the INIT message, users can update their location information. To update location information, send the following message to the server.

```json
{
  "requestType": "UPDATE",
  "data": {
    "longitude": 126.027956,
    "latitude": 36.497979,
    "isUsingNavi": false,
    "meterPerSec": 15.6,
    "direction": 1.3,
    "timestamp": "2024-01-13T12:34:56.789Z"
  }
}
```

The description and type of each field are as follows.
| Field | Description | Type |
|---|---|---|
| longitude | User's longitude | double |
| latitude | User's latitude | double |
| isUsingNavi | Whether the user is using navigation | boolean |
| meterPerSec | User's speed | double |
| direction | User's direction of movement | double |
| timestamp | Time the message was sent | ISO 8601 String |

In the above message, `longitude` and `latitude` represent the user's location information, `isUsingNavi` indicates whether the user is using navigation. `meterPerSec` represents the user's speed, and `direction` represents the user's direction of movement. `timestamp` indicates the time the message was sent.

If the user is using navigation, the application will check the route and provide a wider range of emergency vehicle information to process emergency vehicle information.

**Example of Location Update Request**

```json
{
  "requestType": "UPDATE",
  "data": {
    "longitude": 126.027956,
    "latitude": 36.497979,
    "isUsingNavi": false,
    "meterPerSec": 15.6,
    "direction": 1.3,
    "timestamp": "2024-01-13T12:34:56.789Z"
  }
}
```

The server identifies the user's location based on the user's location information and attempts map matching. The server then returns the map-matched result to the user.

**Example of Location Update Response**

```json
{
  "code": 200,
  "messageType": "RESPONSE",
  "data": {
    "location": {
      "latitude": 36.497979,
      "longitude": 126.027956,
      "direction": 1.3,
      "timestamp": "2024-01-13T12:34:56.789",
      "locationName": "정자일로",
      "confidence": 0.9464
    }
  }
}
```

If map matching is successful, the confidence value is returned with a non-null value. The confidence value (0 ~ 1) indicates the accuracy of the map matching.

### 4. Receiving Emergency Vehicle Information from the EPAS Server

When an emergency vehicle is approaching and there is a high possibility of encountering an emergency vehicle, the server sends the following message.

```javascript
{
  "code": 200,
  "messageType": "ALERT",
  "data": {
    "emergencyEventId": 410,
    "checkPointId": 16,
    "licenseNumber": "947Y1201",
    "vehicleType": "FIRE_TRUCK_MEDIUM",
    "currentPathPoint": 12,
    "pathPoints": [
      {
        "index": 3,
        "location": [127.10739, 37.342598]
      },
      {
        "index": 4,
        "location": [127.108576, 37.342592]
      },
      // ...
      {
        "index": 33,
        "location": [127.116302, 37.346454]
      }
    ]
  }
}
```

The emergency vehicle information, the expected driving route information of the emergency vehicle (partial section), and the current location of the emergency vehicle are included. With this information, the application can anticipate the possibility of encountering an emergency vehicle.

In addition, the emergency vehicle alert is updated every 40 seconds with a 1-second interval. This information allows for more real-time emergency vehicle information.

```json
{
  "code": 200,
  "messageType": "ALERT_UPDATE",
  "data": {
    "licenseNumber": "947Y1201",
    "longitude": 127.109039,
    "latitude": 37.343817
  }
}
```

When the emergency vehicle moves away and there is no need to warn anymore, the server sends the following message.

```json
{
  "code": 200,
  "messageType": "ALERT_END",
  "data": {
    "licenseNumber": "947Y1201"
  }
}
```

### 5. Disconnecting from the EPAS Server

The EPAS server automatically disconnects if the user does not send an UPDATE message for a long time. If the user wants to disconnect, they can simply end the socket communication without sending any messages.

## Emergency Vehicle Driver Application API

Emergency vehicle drivers can use the EPAS application to use the emergency vehicle notification system. If developers wish to develop the emergency vehicle notification system themselves, they can use the EPAS API to implement the emergency vehicle notification system.

### 1. Sign Up and Log In

To use the emergency vehicle notification system of EPAS, you must sign up as an emergency vehicle driver. After signing up through the EPAS application and sending a request for emergency vehicle driver authorization, the EPAS administrator will grant the authorization. For more information, please refer to the [EPAS application]().

### 2. Access Token Issuance and Rotation

For emergency vehicle applications, an Access Token is required to access the server.

**Access Token Issuance Address**

```http
POST https://ajou-epas.xyz:7000/auth/token
```

To obtain an Access Token, send the following request.

```json
{
  "loginType": "EMAIL_PW",
  "username": "username",
  "password": "password"
}
```

The following Access Token is issued in response.

```json
{
  "httpStatus": "OK",
  "code": 200,
  "data": {
    "accessToken": "[access token]",
    "refreshToken": "[refresh token]",
    "accessTokenExpireTime": "2024-05-01T18:28:42.748+00:00",
    "refreshTokenExpireTime": "2024-05-27T14:28:42.748+00:00",
    "ownerEmail": "your email",
    "tokenId": "0e8ac328-0203-4e2b-a432-e43ff63361be"
  }
}
```

Before the Access Token expires, you can renew the Access Token by sending the following request. The refresh token is also renewed at this time.

**Access Token Renewal Address**

```http
POST https://ajou-epas.xyz:7000/api/account/refresh
```

```json
{
  "accessToken": "[access token]",
  "refreshToken": "[refresh token]"
}
```

### 3. EPAS Socket Server Connection

The emergency vehicle socket can be connected to the following address.

**EPAS Connection Address**

```
wss://ajou-epas.xyz:7002/ws/emergency-location
```

After establishing a socket connection, when you are ready to start socket communication, send an INIT message to the server. The INIT message to be sent is as follows. Be sure to include the Access Token in the Authorization header.

```http
Authorization: Bearer [access token]
```

The INIT message to be sent is as follows.

```json
{
  "requestType": "INIT",
  "data": {
    "vehicleId": 3
  }
}
```

When the server receives the INIT message, it identifies the location of the emergency vehicle driver and prepares to send an emergency vehicle notification. Once the preparation is complete, the server sends the following response.

```json
{
  "code": 200,
  "messageType": "RESPONSE",
  "data": {
    "vehicleStatusId": "e6ba5180-7612-bc53-e98e-8ae2fee7a7e3"
  }
}
```

The vehicle number is the ID of the emergency vehicle registered with the EPAS server. You can check the ID of the emergency vehicle through the following API.

**Emergency Vehicle ID Confirmation Address**

```http
GET https://ajou-epas.xyz:7001/api/vehicles/all
```

**Response Example**

```json
{
  "httpStatus": "OK",
  "code": 200,
  "data": [
    {
      "vehicleId": 3,
      "licenceNumber": "123가7899",
      "vehicleType": "MEDIUM_CAR"
    }
  ]
}
```

If there are no registered emergency vehicles, you can register an emergency vehicle through the following API.

**Emergency Vehicle Registration Address**

```http
POST https://ajou-epas.xyz:7001/api/vehicles
```

**Request Example**

```json
{
  "countryCode": "ko-KR",
  "licenceNumber": "123가7808",
  "vehicleType": "MEDIUM_CAR"
}
```

### 4. Update Location on EPAS Server

The emergency vehicle driver can update the location information after successfully connecting to the server through the INIT message. To update the location information, send the following message to the server.

```json
{
  "requestType": "UPDATE",
  "data": {
    "longitude": 127.118621,
    "latitude": 37.379899,
    "isUsingNavi": true,
    "meterPerSec": 15.6,
    "direction": 1.3,
    "timestamp": "2024-01-13T12:34:56.789Z",
    "onEmergencyEvent": false,
    "naviPathId": null,
    "emergencyEventId": null
  }
}
```

`longitude` and `latitude` represent the location information of the emergency vehicle driver, `isUsingNavi` indicates whether the emergency vehicle driver is using navigation. `meterPerSec` represents the speed of the emergency vehicle driver, and `direction` represents the direction of movement of the emergency vehicle driver. `timestamp` indicates the time the message was sent. `onEmergencyEvent` indicates whether the emergency vehicle driver is in an emergency situation. `naviPathId` is the navigation path ID of the emergency vehicle driver. `emergencyEventId` is the ID of the emergency situation in which the emergency vehicle driver is participating.

If the emergency vehicle driver is in an emergency situation, set `onEmergencyEvent` to true and set `emergencyEventId`. In this case, the server sends the location information of the emergency vehicle driver in an emergency situation to other users nearby.

### 5. Registering and Updating Emergency Situations on the EPAS Server

When an emergency situation occurs, you can register an emergency situation by sending a request to the server. This process is as follows.

1. Find the shortest time path to the destination
2. Register an emergency situation
3. Update the current location via a socket

All requests below require emergency vehicle authorization. Therefore, the Access Token must be included in the header.

```http
Authorization: Bearer [access token]
```

**1. Find the shortest time path to the destination**

When an emergency situation occurs, the emergency vehicle driver must find the shortest time path to the destination. In this case, send the following request.

**Shortest Time Path Request Address**

```http
POST https://ajou-epas.xyz:7001/api/emergency/navi/route
```

**Request Example**

```json
{
  "source": "127.116104,37.35866",
  "dest": "127.115186,37.346427",
  "options": "",
  "provider": "OSRM",
  "vehicleId": 3
}
```

`source` and `dest` are the coordinates of the departure and destination, respectively. `options` is the path search option. `provider` is the path search service provider. `vehicleId` is the ID of the emergency vehicle. Currently, only OSRM is supported.

**Response Example**

```javascript
{
    "httpStatus": "OK",
    "code": 200,
    "data": {
        "naviPathId": 722,
        "vehicleId": 3,
        "provider": "OSRM",
        "sourceLocation": {
            "latitude": 127.116107,
            "longitude": 37.358785
        },
        "destLocation": {
            "latitude": 127.115197,
            "longitude": 37.346378
        },
        "queryType": "OSRM",
        "distance": 2042,
        "duration": 218,
        "pathPointSize": 61,
        "currentPathPoint": 0,
        "currentCheckPoint": 19,
        "pathPoint": [
            {
                "index": 0,
                "location": [
                    127.116107,
                    37.358785
                ]
            },
            {
                "index": 1,
                "location": [
                    127.115876,
                    37.358788
                ]
            },
            // ...
        ],
        "checkPoint": [
            {
                "pointIndex": 19,
                "location": [
                    127.118765,
                    37.359178
                ],
                "distance": 404.6402936730819,
                "duration": 0.0
            },
            {
                "pointIndex": 25,
                "location": [
                    127.118969,
                    37.355705
                ],
                "distance": 411.67771177735176,
                "duration": 0.0
            }, // ...
        ]
    }
}
```

The user can check the received path and register an emergency situation using the naviPathId.

**2. Register an Emergency Situation**

When an emergency situation occurs, the emergency vehicle driver sends the following request to register the emergency situation.

**Emergency Situation Registration Address**

```http
POST https://ajou-epas.xyz:7001/api/emergency/event/register
```

**Request Example**

```json
{
  "navigationPathId": 722,
  "vehicleId": 3
}
```

The navigationPathId is the naviPathId received from the shortest path query. vehicleId is the ID of the emergency vehicle.

**Response Example**

```json
{
  "httpStatus": "OK",
  "code": 200,
  "data": {
    "emergencyEventId": 699,
    "navigationPathId": 722,
    "issuerEmail": "jsm5315@gmail.com",
    "isActive": true,
    "createdDate": "2024-01-13T12:34:56.789Z",
    "endedDate": null,
    "warnRecordDtos": []
  }
}
```

The emergencyEventId is the ID of the registered emergency situation. The emergency situation is registered at the EPAS server.

**3. Update the Current Location via a Socket**

After registering the emergency situation, the emergency vehicle driver can update the location information of the emergency vehicle driver through the socket communication established earlier. To update the location information, send the following message to the server.

```json
{
  "requestType": "UPDATE",
  "data": {
    "longitude": 127.118621,
    "latitude": 37.379899,
    "isUsingNavi": true,
    "meterPerSec": 15.6,
    "direction": 1.3,
    "timestamp": "2024-01-13T12:34:56.789Z",
    "onEmergencyEvent": true,
    "naviPathId": 722,
    "emergencyEventId": 699
  }
}
```

In the message above, set `onEmergencyEvent` to true and set `naviPathId` and `emergencyEventId`. In this case, the server sends the location information of the emergency vehicle driver in an emergency situation to other users nearby.

### 6. Ending an Emergency Situation

When the emergency situation is over, send the following request to end the emergency situation.

**End Emergency Situation Address**

```http
POST https://ajou-epas.xyz:7001/api/emergency/event/end
```

**Request Example**

```json
{
  "emergencyEventId": 699
}
```

The emergencyEventId is the emergencyEventId received from the emergency situation registration. The server does not send the location information of the emergency vehicle driver whose emergency situation has ended to other users nearby.

**Response Example**

```json
{
  "httpStatus": "OK",
  "code": 200,
  "data": "Event Ended Successfully"
}
```

Emergency situations are automatically ended when a new emergency situation is registered without ending the previous emergency situation.

Now have to modify some fields in the data sent via socket communication. Set `onEmergencyEvent` to false, and set `naviPathId` and `emergencyEventId` to null.
