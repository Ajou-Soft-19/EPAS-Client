# EPAS-Embeded-Client

## 프로젝트 소개

이 리포지토리는 임베디드 시스템을 위한 EPAS 클라이언트 시스템을 구현한 코드입니다. EPAS는 처음에는 네비게이션에 내장하기 위한 플러그인을 목표로 개발되었으나, 네비게이션을 사용하지 않는(또는 사용하기 힘든) 사용자들을 위해 별도의 임베디드 장치로 구현되었습니다.
EPAS 클라이언트 디바이스는 큰 컴퓨팅 파워가 필요하지 않으며, GPS와 네트워크 통신 기능만 있으면 됩니다. 따라서 아두이노와 같은 저성능 싱글 쓰레드 임베디드 시스템에서도 충분히 동작 가능합니다. 이를 통해 기존의 차량들은 값싼 비용으로 EPAS 시스템을 사용할 수 있고, 스마트폰이나 네비게이션 시스템이 차량에 내장된 경우에는 별도의 하드웨어 추가 없이 소프트웨어 업데이트만으로 EPAS 시스템을 사용할 수 있습니다.

|                                                EPAS 회로 구성                                            |             EPAS Client Divice Prototype               |
| :------------------------------------------------------------------------------------------------: | :----------------------------------------------: |
| <img src="https://github.com/Ajou-Soft-19/EPAS-Embeded-Client/assets/32717522/db2253ca-8453-4697-8732-e4e98be4027a" alt="EPAS" style="display: block; margin-left: auto; margin-right: auto;" /> | <img src="https://github.com/Ajou-Soft-19/EPAS-Embeded-Client/assets/32717522/06179948-e7e7-4f62-894e-7a36c9a6e1fe" alt="Additional Information" style="display: block; margin-left: auto; margin-right: auto;" /> |

위 사진은 개발된 EPAS 클라이언트 디바이스의 프로토타입 사진입니다. 이 디바이스에 사용된 하드웨어는 아래와 같습니다.

| 번호 | 이름             | 수량 | 가격 (USD) |
|------|-----------------|------|------------|
| 1    | NodeMCU V3      | 1    | $1.8       |
| 2    | GPS 모듈 (Neo-7M) | 1    | $5.7       |
| 3    | LCD (16x2)      | 1    | $1.0       |
| 4    | DFPlayer Mini   | 1    | $0.8       |
| 5    | 스피커           | 1    | $1.3       |
| 6    | Micro SD 카드(16GB)   | 1    | $3.0       |

총 비용 : 약 $13.6

프로토타입은 개발 및 테스트 목적으로 제작되었으며, 실제 제품은 도매를 통해 더 작고 저렴한 하드웨어를 사용할 수 있고 손바닥 크기 정도의 크기로 제작할 수 있습니다.

## 동작 방식

- **LCD 경고 사진 붙이기**

EPAS 클라이언트 디바이스는 GPS 모듈을 통해 자신의 현재 위치를 확인하고 소켓 통신을 통해 [EPAS 차량 모니터링 서버](https://github.com/Ajou-Soft-19/spring-socket-server)에 현재 위치를 전송합니다. EPAS 서버는 이를 기준으로 주위에 응급차량이 지나가는지 확인하고, 응급차량과 마주칠 것으로 예상된다면 클라이언트 디바이스에 경고 메시지를 전송합니다. 클라이언트 디바이스는 이를 받아 LCD에 경고 메시지를 출력하고, 동시에 DFPlayer Mini를 통해 경고음을 출력합니다. 이때 사용자는 응급차량의 라이선스 번호와 응급차량이 접근하고 있는 방향을 LCD 모니터와 스피커를 통해 확인할 수 있습니다.

만약 주위에 복수의 응급차량이 있다면, EPAS 클라이언튼는 차례대로 경고 메시지를 출력하며 사용자에게 알립니다. 사용자는 이를 통해 주변 상황을 파악하고, 응급차량이 지나가는 길을 피할 수 있습니다.

## 기대 효과

EPAS 클라이언트 디바이스는 다음과 같은 효과를 기대할 수 있습니다.

1. **사용자의 안전 보장**: 응급차량의 접근을 미리 인지하고 피할 수 있어, 사용자의 안전을 보장합니다.
2. **기존 차량에 대한 접근성**: 기존 차량에도 저렴한 비용으로 EPAS 시스템을 사용할 수 있어, 보급성이 높습니다.
3. **사용자 편의성**: 사용자는 응급차량의 접근 방향을 LCD와 스피커를 통해 확인할 수 있어, 주변 상황을 파악하기 쉽습니다.
4. **교통 흐름 개선**: 일반 차량 운전자가 응급차량의 접근을 미리 인지하고 대응할 수 있게 되면, 응급차량을 위한 길을 효율적으로 만들어 줄 수 있습니다. 이는 교통 흐름을 개선하고, 교통 혼잡을 줄이는 데 기여할 수 있습니다.
5. **사회적 인식 개선**: EPAS 시스템의 보급과 사용을 통해 운전자들 사이에 응급차량에 대한 인식이 개선될 수 있습니다. 응급차량에 길을 양보하는 문화가 자연스럽게 정착됨으로써, 사회 전체의 응급 상황 대응 능력이 강화될 수 있습니다.
6. **기술적 확장성**: EPAS 클라이언트 디바이스는 기존의 차량에 쉽게 설치할 수 있는 저렴한 솔루션이지만, 향후 다양한 기술적 확장이 가능합니다. 예를 들어, 인공지능을 활용하여 사용자의 운전 습관을 분석하고, 응급차량의 접근에 대한 더 효과적인 대응 방안을 제시할 수 있습니다.

EPAS 시스템을 사회 기반 시스템에 도입한다면 값싼 비용으로 응급차량의 통행에 큰 도움을 줄 수 있으며, 사용자들의 안전 운행에 기여할 수 있습니다.
