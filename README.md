22학번 이영흔

**1) ~ 7) 완료, 8)~9) 부분완료 (버그 발생), 11) 건물, 유닛 초기에 모두 배치, 건설/생산 없음,유닛 행동 X

- **건설 시 2X2 커서 변경이 어려워 1X1 로 진행**
- **글씨가 길어 꼭 실행할 때 전체화면 부탁드립니다.**

  
- **추가기능** <br>**시스템 메시지 창에 중요도를 나눠 색깔 변경**


## -샌드웜 이동 방식
  1. 가장 가까운 하베스터를 찾는 형식으로 맵에서 각 하베스터와 샌드웜 간의 거리를 계산하고, 가장 가까운 하베스터를 찾아갑니다.
  2. 먼저 맵의 모든 위치를 순차적으로 검사합니다.
  3. 각 위치에서 H가 있으면 그 위치와 샌드웜의 거리를 계산합니다.
  4. 샌드웜과 각 하베스터 사이의 행과 열 차이의 절대값 합을 계산하고, 그 중 가장 작은 거리를 가진 하베스터를 찾습니다.
  5. 가장 가까운 하베스터의 위치를 POSITION 구조체로 반환합니다.