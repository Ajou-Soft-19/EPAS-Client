# EPAS API 사용 방법

## 일반 유저 어플리케이션

### 1. EPAS API

EPAS Client는 별다른 회원정보를 요구하지 않습니다. 사용자는 EPAS 서버와 소켓 통신을 통해 쉽게 응급 차량 데이터를 받을 수 있습니다. EPAS 서버는 전달받은 GPS 정보를 기반으로 사용자의 위치를 파악하고, 알림이 필요한 경우에만 알림을 전송합니다.
개발자는 API에서 제공하는 응답 JSON을 통해 사용자에게 적절한 알림을 제공할 수 있습니다.

### 2. EPAS 서버 접속

EPAS 서버에 접속하기 위해서 다음 API 주소를 사용합니다.

**EPAS 접속 주소**

```
wss://ajou-epas.xyz:7002/ws/my-location
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
    "code": 200,
    "messageType": "RESPONSE",
    "data": {
        "vehicleStatusId": "e6ba5180-7612-bc53-e98e-8ae2fee7a7e3"
    }
}
```

vehicleStatusId는 익명 사용자를 식별하기 위한 임시 고유 식별 ID입니다.

### 3. EPAS 서버에 위치 업데이트

사용자는 INIT 메시지를 통해 성공적으로 연결을 맺은 후에 위치 정보를 업데이트할 수 있습니다. 위치 정보를 업데이트하기 위해서는 다음과 같은 메시지를 서버에 전송합니다.

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

각 필드의 설명과 타입은 다음과 같습니다.
| 필드 | 설명 | 타입 |
|---|---|---|
| longitude | 사용자의 경도 | double |
| latitude | 사용자의 위도 | double |
| isUsingNavi | 사용자가 네비게이션을 사용하고 있는지 여부 | boolean |
| meterPerSec | 사용자의 속도 | double |
| direction | 사용자의 이동 방향 | double |
| timestamp | 메시지를 전송한 시간 | ISO 8601 String |

위의 메시지에서 `longitude`와 `latitude`는 사용자의 위치 정보를 나타내며, `isUsingNavi`는 사용자가 네비게이션을 사용하고 있는지 여부를 나타냅니다. `meterPerSec`는 사용자의 속도를 나타내며, `direction`은 사용자의 이동 방향을 나타냅니다. `timestamp`는 메시지를 전송한 시간을 나타냅니다.

네비게이션을 사용하고 있을 경우 어플리케이션이 경로를 확인하고 응급차량 정보를 처리할 수 있도록 더 넓은 범위의 응급차량 정보를 제공합니다.

**위치 업데이트 요청 예시**

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

서버는 사용자의 위치 정보를 통해 사용자가 어떤 도로에 위치하고 있는지 파악하고, 맵 매칭을 시도합니다. 이후 맵 매칭된 결과를 사용자에게 반환해줍니다.

**위치 업데이트 응답 예시**

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

맵매칭에 성공하면 confidence 값이 null이 아닌 값으로 반환됩니다. confidence 값은(0 ~ 1) 맵매칭의 정확도를 나타냅니다.

### 4. EPAS 서버로부터 응급차량 정보 수신

주위에 응급차량이 접근하고 있고, 응급 차량과 마주칠 가능성이 높은 경우 서버는 다음과 같은 메시지를 보냅니다.

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
      // ...
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

응급차량 정보와 응급차량의 예상 주행 경로 정보(일부 구간), 응급 차량의 현재 위치 등이 포함되어 있습니다. 이 정보를 통해 어플리케이션은 응급차량과의 마주칠 가능성을 미리 파악할 수 있습니다.

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

## 응급차량 운전자 어플리케이션

응급차량 운전장의 경우 EPAS 어플리케이션을 이용해 응급차량 알림 시스템을 사용할 수 있습니다. 개발자가 직접 개발을 희망할 경우 EPAS API를 사용하여 응급차량 알림 시스템을 구현할 수 있습니다.

### 1. 회원가입 및 로그인

EPAS의 응급차량 알림 시스템을 사용하기 위해서는 응급차량 운전자로 회원가입을 해야 합니다. EPAS 어플리케이션을 통해 회원가입을 하고 응급차량 운전자 권한 요청을 보내면 EPAS 관리자가 권한을 부여합니다. 자세한 내용은 [EPAS 어플리케이션]()을 참고해주세요.

### 2. Access Token 발급 및 rotate

응급차량 어플리케이션의 경우 서버에 접근하기 위한 Access Token을 발급받아야 합니다.

**Access Token 발급 주소**

```
POST https://ajou-epas.xyz:7000/auth/token
```

Access Token을 발급받기 위해서는 다음과 같은 요청을 보냅니다.

```json
{
  "loginType": "EMAIL_PW",
  "username": "username",
  "password": "password"
}
```

응답으로 다음과 같은 Access Token이 발급됩니다.

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

Access Token이 만료되기 전에 다음과 같은 요청을 보내면 Access Token을 갱신할 수 있습니다. 이때 refresh token도 함께 갱신됩니다.

**Access Token 갱신 주소**

```
POST https://ajou-epas.xyz:7000/api/account/refresh
```

```json
{
    "accessToken": "[access token]",
    "refreshToken": "[refresh token]",
}
```

### 3. EPAS 소켓 서버 접속

응급 차량 소켓은 다음 주소로 접속할 수 있습니다.

**EPAS 접속 주소**

```
wss://ajou-epas.xyz:7002/ws/emergency-location
```

소켓 연결을 맺은 이후 소켓 통신을 시작할 준비가 되면 서버에 INIT 메시지를 전송합니다. 이때 Authorization 헤더에 Access Token을 포함시켜야 합니다.

```http
Authorization: Bearer [access token]
```

전송하는 INIT 메시지는 다음과 같습니다.

```json
{
  "requestType": "INIT",
  "data": {
    "vehicleId": 3
  }
}
```

서버는 INIT 메시지를 전달 받으면 응급차량 운전자의 위치를 파악하고, 응급차량 알림을 보낼 준비를 합니다. 준비를 성공적으로 마치면 서버는 다음과 같은 응답을 전송합니다.

```json
{
    "code": 200,
    "messageType": "RESPONSE",
    "data": {
        "vehicleStatusId": "e6ba5180-7612-bc53-e98e-8ae2fee7a7e3"
    }
}
```

차량 번호는 EPAS 서버에 등록된 응급차량의 ID입니다. 아래 API를 통해 응급차량의 ID를 확인할 수 있습니다.

**응급차량 ID 확인 주소**

```
GET https://ajou-epas.xyz:7001/api/vehicles/all
```

**응답 예시**

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

만약 등록된 응급차량이 없다면 아래 API를 통해 응급차량을 등록할 수 있습니다.

**응급차량 등록 주소**

```
POST https://ajou-epas.xyz:7001/api/vehicles
```

**요청 예시**

```json
{
    "countryCode": "ko-KR",
    "licenceNumber": "123가7808",
    "vehicleType": "MEDIUM_CAR"
}
```

### 4. EPAS 서버로 위치 정보 업데이트

응급차량 운전자는 INIT 메시지를 통해 성공적으로 연결을 맺은 후에 위치 정보를 업데이트할 수 있습니다. 위치 정보를 업데이트하기 위해서는 다음과 같은 메시지를 서버에 전송합니다.

```json
{
    "requestType": "UPDATE",
    "data": {
        "vehicleId": 3,
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

vehicleId는 EPAS 서버에 등록된 응급차량의 ID입니다. `longitude`와 `latitude`는 응급차량 운전자의 위치 정보를 나타내며, `isUsingNavi`는 응급차량 운전자가 네비게이션을 사용하고 있는지 여부를 나타냅니다. `meterPerSec`는 응급차량 운전자의 속도를 나타내며, `direction`은 응급차량 운전자의 이동 방향을 나타냅니다. `timestamp`는 메시지를 전송한 시간을 나타냅니다. `onEmergencyEvent`는 응급차량 운전자가 응급 상황에 있는지 여부를 나타냅니다. `naviPathId`는 응급차량 운전자의 네비게이션 경로 ID입니다. `emergencyEventId`는 응급차량 운전자가 참여 중인 응급 상황 ID입니다.

만약 응급 상황에 있는 경우 `onEmergencyEvent`를 true로 설정하고, `emergencyEventId`를 설정해야 합니다. 이때 서버는 응급 상황에 있는 응급차량 운전자의 위치 정보를 주변의 다른 사용자에게 전송합니다.

### 5. EPAS 서버에 응급 상황 등록 및 업데이트

응급 상황이 발생하면 API로 요청을 보내 응급 상황을 등록할 수 있습니다. 이때 과정은 아래와 같습니다.

1. 목적지까지의 최단 경로 조회
2. 응급 상황 등록
3. 소켓을 통해 현재 위치 업데이트

아래 요청은 모두 응급차량 권한이 필요합니다. 따라서 Access Token을 헤더에 포함시켜야 합니다.

```http
Authorization: Bearer [access token]
```

**1. 목적지까지의 최단 경로 조회**

응급 상황이 발생하면 응급차량 운전자는 목적지까지의 최단 경로를 조회해야 합니다. 이때 다음과 같은 요청을 보냅니다.

**최단 경로 조회 주소**

```
POST https://ajou-epas.xyz:7001/api/emergency/navi/route
```

**요청 예시**

```json
{
    "source": "127.116104,37.35866",
    "dest": "127.115186,37.346427",
    "options": "",
    "provider": "OSRM",
    "vehicleId": 3
}
```

source와 dest는 각각 출발지와 목적지의 좌표입니다. options는 경로 탐색 옵션입니다. provider는 경로 탐색 서비스 제공자입니다. vehicleId는 응급차량의 ID입니다. 현재는 OSRM만 지원합니다.

**응답 예시**

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

사용자는 응답으로 받은 경로를 확인하고 naviPathId를 통해 응급 상황을 등록할 수 있습니다.

**2. 응급 상황 등록**

응급 상황이 발생하면 응급차량 운전자는 다음과 같은 요청을 보내 응급 상황을 등록합니다.

**응급 상황 등록 주소**

```
POST https://ajou-epas.xyz:7001/api/emergency/event/register
```

**요청 예시**

```json
{
    "navigationPathId": 722,
    "vehicleId": 3
}
```

navigationPathId는 최단 경로 조회에서 받은 naviPathId입니다. vehicleId는 응급차량의 ID입니다.

**응답 예시**

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

이제 응급 차량이 소켓 통신을 통해 위치를 업데이트하면 서버는 응급 상황에 있는 응급차량의 위치 정보를 주변의 다른 사용자에게 전송합니다.

**3. 소켓을 통해 현재 위치 업데이트**

이전에 맺은 소켓 통신을 통해 응급차량 운전자의 위치 정보를 업데이트할 수 있습니다. 위치 정보를 업데이트하기 위해서는 다음과 같은 메시지를 서버에 전송합니다.

```json
{
    "requestType": "UPDATE",
    "data": {
        "vehicleId": 3,
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

위의 메시지에서 `onEmergencyEvent`를 true로 설정하고, `naviPathId`와 `emergencyEventId`를 설정해야 합니다. 이때 서버는 응급 상황에 있는 응급차량 운전자의 위치 정보를 주변의 다른 사용자에게 전송합니다.

### 6. 응급 상황 종료

응급 상황이 종료되면 다음과 같은 요청을 보내 응급 상황을 종료합니다.

**응급 상황 종료 주소**

```
POST https://ajou-epas.xyz:7001/api/emergency/event/end
```

**요청 예시**

```json
{
    "emergencyEventId": 699
}
```

emergencyEventId는 응급 상황 등록에서 받은 emergencyEventId입니다. 이때 서버는 응급 상황이 종료된 응급차량 운전자의 위치 정보를 주변의 다른 사용자에게 전송하지 않습니다.

**응답 예시**

```json
{
    "httpStatus": "OK",
    "code": 200,
    "data": "Event Ended Successfully"
}
```

응급 상황은 한 차량 당 1개만 활성화되므로 응급 상황을 종료하지 않고 새로운 응급 상황을 등록하면 이전 응급 상황은 자동으로 종료됩니다.

이제 소켓 통신으로 보내는 데이터에서 몇 가지 필드를 수정하면 됩니다. `onEmergencyEvent`를 false로 설정하고, `naviPathId`와 `emergencyEventId`를 null로 설정하면 됩니다.
