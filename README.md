# TutumHW
Tutum 스마트 안전모

![아키텍처](/image/hw_arc.PNG)

# 프로젝트 목적

# 사용 센서 및 보드 ..etc

1. NanoBle33Sense https://docs.arduino.cc/hardware/nano-33-ble-sense/
2. LSM9DS1 https://pdf1.alldatasheet.com/datasheet-pdf/view/1242998/STMICROELECTRONICS/LSM9DS1.html
3. HTS221 https://www.st.com/resource/en/datasheet/hts221.pdf
4. LPS22 https://www.st.com/resource/en/datasheet/dm00140895.pdf
5. SingleTact Capacitive https://www.singletact.com/SingleTact_Datasheet.pdf
6. HC-06 https://pdf1.alldatasheet.com/datasheet-pdf/view/1179032/ETC1/HC-06.html


# 실험
<img src="/image/Theil_Sen1.PNG" width="400px" height="300px"></img>
<img src="/image/Theil_Sen2.PNG" width="400px" height="300px"></img><br/>
<img src="/image/Beacon_Thresh1.PNG"></img><br/>
>Beacon이 설치된 게이트를 3M 거리에서 통과한 후 3M거리까지 걸었을 때 수집된 Beacon Data들의 RSSI분포를 Theil-Sen 회귀를 통해 2차 함수로 근사한 그래프입니다.
아래의 통계는 특정 dB를 Threshold로 Theil-sen회귀로 만들어진 이차 함수의 극값을 판별했을 때 지나갔다고 판별이 됐는지, 아닌지에 대한 통계입니다.
-52 ~ -58 dB에서 98.88888889%로 Beacon을 활용하여 게이트를 통과했는지 판별할 수 있음을 알 수 있습니다.
이를 통해 근로자가 건설 현장에 투입(진입)했는지 확인할 수 있습니다.



<img src="/image/Capa_Test.PNG"></img><br/>



# 참고

Arduino의 Nano Ble 33 sense library를 수정하여 사용하였습니다.