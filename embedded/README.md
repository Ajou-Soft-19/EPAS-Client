# EPAS-Embeded-Client

## Project Introduction

This repository contains the code for the EPAS client system for embedded systems. EPAS was initially developed as a plugin to be embedded in navigation, but it was also implemented as a separate embedded device for users who do not use (or have difficulty using) navigation. The EPAS client device does not require a large computing power, only GPS and network communication functions. Therefore, it can run sufficiently on low-performance single-threaded embedded systems such as Arduino. This allows existing vehicles to add the EPAS system at a low cost, and if a smartphone or vehicle that has navigation system, the EPAS can be used with only a software update without additional hardware.

Source code can be found at this link: [EPAS-Embeded-Client](https://github.com/Ajou-Soft-19/EPAS-Client/blob/main/embedded/gsc_node_mcu.ino)

|                                          EPAS Implementaion                                            |             EPAS Client Divice Prototype               |
| :------------------------------------------------------------------------------------------------: | :----------------------------------------------: |
| <img src="https://github.com/Ajou-Soft-19/EPAS-Embeded-Client/assets/32717522/db2253ca-8453-4697-8732-e4e98be4027a" alt="EPAS" style="display: block; margin-left: auto; margin-right: auto;" /> | <img src="https://github.com/Ajou-Soft-19/EPAS-Client/assets/32717522/ec86e577-3862-43d4-927b-14db2f0a534d" alt="Additional Information" style="display: block; margin-left: auto; margin-right: auto;" /> |

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
