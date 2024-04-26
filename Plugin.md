# EPAS System 사용 방법

## Client

### 1. EPAS API

EPAS Client는 별다른 회원정보를 요구하지 않습니다. 사용자는 EPAS 서버와 소켓 통신을 통해 쉽게 응급 차량 데이터를 받을 수 있습니다. EPAS 서버는 전달받은 GPS 정보를 기반으로 사용자의 위치를 파악하고, 알림이 필요한 경우에만 알림을 전송합니다. 
개발자는 API에서 제공하는 응답 JSON을 통해 사용자에게 적절한 알림을 제공할 수 있습니다.

### 2. EPAS 서버 접속

EPAS 서버에 접속하기 위해서 다음 API 주소를 사용합니다.

**EPAS 접속 주소**
```
wss://ajou-epas.xyz:7002
```

소켓 연결을 맺은 이후 소켓 통신을 시작할 준비가 되면 서버에 INIT 메시지를 전송합니다. 전송하는 INIT 메시지는 다음과 같습니다.
```json
{
  "requestType": "INIT"
}
```

서버는 INIT 메시지를 전달 받으면 사용자의 위치를 파악하고, 응급차량 알림을 보낼 준비를 합니다. 준비를 성공적으로 마치면 서버는 다음과 같은 응답을 전송합니다.
```json
{
  "responseType": "INIT",
  "status": "SUCCESS"
}
```

### 3. EPAS 서버에 위치 업데이트

사용자는 INIT 메시지를 통해 성공적으로 연결을 맺은 후에 위치 정보를 업데이트할 수 있습니다. 위치 정보를 업데이트하기 위해서는 다음과 같은 메시지를 서버에 전송합니다.
```json
{
  "requestType": "UPDATE",
  "data": {
    "longitude": longitude,
    "latitude": latitude,
    "isUsingNavi": isUsingNavi,
    "meterPerSec": meterPerSec,
    "direction": direction,
    "timestamp": timestamp
  }
}
```

위의 메시지에서 `longitude`와 `latitude`는 사용자의 위치 정보를 나타내며, `isUsingNavi`는 사용자가 네비게이션을 사용하고 있는지 여부를 나타냅니다. `meterPerSec`는 사용자의 속도를 나타내며, `direction`은 사용자의 이동 방향을 나타냅니다. `timestamp`는 메시지를 전송한 시간을 나타냅니다.

네비게이션을 사용하고 있을 경우 어플리케이션이 경로를 확인하고 응급차량 정보를 처리할 수 있도록 더 넓은 범위의 응급차량 정보를 제공합니다.

### 4. EPAS 서버로부터 응급차량 정보 수신

주위에 응급차량이 접근하고 있고, 응급 차량과 마주칠 가능성이 높은 경우 서버는 다음과 같은 메시지를 보냅니다.
```json
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
        "location": [
          127.10739,
          37.342598
        ]
      },
      {
        "index": 4,
        "location": [
          127.108576,
          37.342592
        ]
      },
      ...
      {
        "index": 33,
        "location": [
          127.116302,
          37.346454
        ]
      }
    ]
  }
}
```
응급차량 정보와 응급차량의 예상 주행 경로 정보, 응급 차량의 현재 위치 등이 포함되어 있습니다. 이 정보를 통해 어플리케이션은 응급차량과의 마주칠 가능성을 미리 파악할 수 있습니다.

또한 응급차량 알림은 한 경고마다 40초간 1초 간격으로 업데이트됩니다. 이 정보를 통해 조금 더 실시간적인 응급차량 정보를 제공할 수 있습니다.

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

응급 차량이 멀어져 더 이상 경고할 필요가 없을 경우 서버는 다음과 같은 메시지를 보냅니다.
```json
{
  "code": 200,
  "messageType": "ALERT_END",
  "data": {
    "licenseNumber": "947Y1201"
  }
}
```

### 5. EPAS 서버 연결 종료

EPAS 서버는 사용자가 오랫동안 UPDATE 메시지를 보내지 않을 경우 자동으로 연결을 종료합니다. 만약 사용자가 연결을 종료하고 싶다면 별다른 메시지를 보낼 필요 없이 소켓 통신을 종료하면 됩니다. 