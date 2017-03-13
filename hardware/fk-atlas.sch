EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:conservify
LIBS:fk-atlas-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L FEATHER U2
U 1 1 5887CF41
P 5550 3850
F 0 "U2" H 5150 2400 60  0000 C CNN
F 1 "FEATHER" H 5550 3850 60  0000 C CNN
F 2 "conservify:feather" H 5550 3850 60  0001 C CNN
F 3 "" H 5550 3850 60  0001 C CNN
	1    5550 3850
	1    0    0    -1  
$EndComp
$Comp
L ATLAS_SENSOR U4
U 1 1 5887CF5A
P 9650 1000
F 0 "U4" H 9200 650 60  0000 C CNN
F 1 "ATLAS_SENSOR" H 9700 950 60  0000 C CNN
F 2 "atlas_scientific:atlas_sensor_board" H 9650 1000 60  0001 C CNN
F 3 "" H 9650 1000 60  0001 C CNN
	1    9650 1000
	1    0    0    -1  
$EndComp
$Comp
L ATLAS_SENSOR U5
U 1 1 5887CF7F
P 9650 1750
F 0 "U5" H 9200 1400 60  0000 C CNN
F 1 "ATLAS_SENSOR" H 9700 1700 60  0000 C CNN
F 2 "atlas_scientific:atlas_sensor_board" H 9650 1750 60  0001 C CNN
F 3 "" H 9650 1750 60  0001 C CNN
	1    9650 1750
	1    0    0    -1  
$EndComp
$Comp
L ATLAS_SENSOR U6
U 1 1 5887CF9E
P 9650 2500
F 0 "U6" H 9200 2150 60  0000 C CNN
F 1 "ATLAS_SENSOR" H 9700 2450 60  0000 C CNN
F 2 "atlas_scientific:atlas_sensor_board" H 9650 2500 60  0001 C CNN
F 3 "" H 9650 2500 60  0001 C CNN
	1    9650 2500
	1    0    0    -1  
$EndComp
$Comp
L ATLAS_SENSOR U7
U 1 1 5887CFC8
P 9650 3250
F 0 "U7" H 9200 2900 60  0000 C CNN
F 1 "ATLAS_SENSOR" H 9700 3200 60  0000 C CNN
F 2 "atlas_scientific:atlas_sensor_board" H 9650 3250 60  0001 C CNN
F 3 "" H 9650 3250 60  0001 C CNN
	1    9650 3250
	1    0    0    -1  
$EndComp
$Comp
L VISO U1
U 1 1 5887CFFA
P 4150 1050
F 0 "U1" H 3750 350 60  0000 C CNN
F 1 "VISO" H 4150 900 60  0000 C CNN
F 2 "atlas_scientific:voltage_isolator" H 4150 1050 60  0001 C CNN
F 3 "" H 4150 1050 60  0001 C CNN
	1    4150 1050
	1    0    0    -1  
$EndComp
Text Label 5000 900  0    60   ~ 0
ISO_V
Text Label 5000 1000 0    60   ~ 0
ISO_GND
Text Label 8500 1100 0    60   ~ 0
ISO_V
Text Label 8500 1200 0    60   ~ 0
ISO_GND
Text Label 8500 1850 0    60   ~ 0
ISO_V
Text Label 8500 1950 0    60   ~ 0
ISO_GND
Text Label 8500 2700 0    60   ~ 0
ISO_GND
Text Label 8500 2600 0    60   ~ 0
ISO_V
$Comp
L CONN_01X02 P2
U 1 1 5887DF80
P 10850 1050
F 0 "P2" H 10850 1200 50  0000 C CNN
F 1 "CONN_01X02" V 10950 1050 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x02_Pitch2.54mm" H 10850 1050 50  0001 C CNN
F 3 "" H 10850 1050 50  0000 C CNN
	1    10850 1050
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P3
U 1 1 5887E09B
P 10850 1800
F 0 "P3" H 10850 1950 50  0000 C CNN
F 1 "CONN_01X02" V 10950 1800 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x02_Pitch2.54mm" H 10850 1800 50  0001 C CNN
F 3 "" H 10850 1800 50  0000 C CNN
	1    10850 1800
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P4
U 1 1 5887E0CE
P 10850 2550
F 0 "P4" H 10850 2700 50  0000 C CNN
F 1 "CONN_01X02" V 10950 2550 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x02_Pitch2.54mm" H 10850 2550 50  0001 C CNN
F 3 "" H 10850 2550 50  0000 C CNN
	1    10850 2550
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P5
U 1 1 5887E104
P 10850 3300
F 0 "P5" H 10850 3450 50  0000 C CNN
F 1 "CONN_01X02" V 10950 3300 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x02_Pitch2.54mm" H 10850 3300 50  0001 C CNN
F 3 "" H 10850 3300 50  0000 C CNN
	1    10850 3300
	1    0    0    -1  
$EndComp
NoConn ~ 7400 1500
NoConn ~ 7400 1600
$Comp
L +5V #PWR01
U 1 1 5887F0BD
P 3000 900
F 0 "#PWR01" H 3000 750 50  0001 C CNN
F 1 "+5V" H 3000 1040 50  0000 C CNN
F 2 "" H 3000 900 50  0000 C CNN
F 3 "" H 3000 900 50  0000 C CNN
	1    3000 900 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5887F11A
P 2950 1000
F 0 "#PWR02" H 2950 750 50  0001 C CNN
F 1 "GND" H 2950 850 50  0000 C CNN
F 2 "" H 2950 1000 50  0000 C CNN
F 3 "" H 2950 1000 50  0000 C CNN
	1    2950 1000
	0    1    1    0   
$EndComp
Text Label 2900 1200 0    60   ~ 0
SPE_SEL_1
Text Label 2900 1300 0    60   ~ 0
SPE_SEL_2
Text Label 2900 1450 0    60   ~ 0
SPE_RX
Text Label 2900 1550 0    60   ~ 0
SPE_TX
$Comp
L +5V #PWR03
U 1 1 5887F61C
P 8150 3350
F 0 "#PWR03" H 8150 3200 50  0001 C CNN
F 1 "+5V" H 8150 3490 50  0000 C CNN
F 2 "" H 8150 3350 50  0000 C CNN
F 3 "" H 8150 3350 50  0000 C CNN
	1    8150 3350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 5887F648
P 8550 3450
F 0 "#PWR04" H 8550 3200 50  0001 C CNN
F 1 "GND" H 8550 3300 50  0000 C CNN
F 2 "" H 8550 3450 50  0000 C CNN
F 3 "" H 8550 3450 50  0000 C CNN
	1    8550 3450
	0    1    1    0   
$EndComp
Text Label 8300 3150 0    60   ~ 0
EC_RX
Text Label 8300 3250 0    60   ~ 0
EC_TX
NoConn ~ 6200 4100
$Comp
L GND #PWR05
U 1 1 5887FCB1
P 4550 3900
F 0 "#PWR05" H 4550 3650 50  0001 C CNN
F 1 "GND" H 4550 3750 50  0000 C CNN
F 2 "" H 4550 3900 50  0000 C CNN
F 3 "" H 4550 3900 50  0000 C CNN
	1    4550 3900
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR06
U 1 1 5887FE2D
P 6650 4200
F 0 "#PWR06" H 6650 4050 50  0001 C CNN
F 1 "+5V" H 6650 4340 50  0000 C CNN
F 2 "" H 6650 4200 50  0000 C CNN
F 3 "" H 6650 4200 50  0000 C CNN
	1    6650 4200
	1    0    0    -1  
$EndComp
NoConn ~ 6200 4000
NoConn ~ 4850 3800
$Comp
L +3V3 #PWR07
U 1 1 58880076
P 4500 3700
F 0 "#PWR07" H 4500 3550 50  0001 C CNN
F 1 "+3V3" H 4500 3840 50  0000 C CNN
F 2 "" H 4500 3700 50  0000 C CNN
F 3 "" H 4500 3700 50  0000 C CNN
	1    4500 3700
	1    0    0    -1  
$EndComp
NoConn ~ 4850 3600
Text Label 6300 4900 0    60   ~ 0
SPE_SEL_1
Text Label 6300 4800 0    60   ~ 0
SPE_SEL_2
Text Label 4400 5000 0    60   ~ 0
SPE_RX
Text Label 4400 4900 0    60   ~ 0
SPE_TX
Text Label 6300 4600 0    60   ~ 0
EC_RX
Text Label 6300 4500 0    60   ~ 0
EC_TX
NoConn ~ 4850 5100
NoConn ~ 4850 4300
NoConn ~ 4850 4400
NoConn ~ 4850 4500
NoConn ~ 6200 4300
NoConn ~ 6200 4400
NoConn ~ 6200 4700
$Comp
L SERIAL_PORT_EXPANDER U3
U 1 1 58C69C65
P 6400 1050
F 0 "U3" H 5650 450 60  0000 C CNN
F 1 "SERIAL_PORT_EXPANDER" H 6400 1050 60  0000 C CNN
F 2 "conservify:serial_port_expander" H 6400 1050 60  0001 C CNN
F 3 "" H 6400 1050 60  0001 C CNN
	1    6400 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 900  5350 900 
Wire Wire Line
	5350 1000 4950 1000
Wire Wire Line
	4950 1450 5150 1450
Wire Wire Line
	8500 1100 8900 1100
Wire Wire Line
	8900 1200 8500 1200
Wire Wire Line
	8500 2600 8900 2600
Wire Wire Line
	8900 2700 8500 2700
Wire Wire Line
	8500 1950 8900 1950
Wire Wire Line
	8500 1850 8900 1850
Wire Wire Line
	10650 3350 10550 3350
Wire Wire Line
	10550 3250 10650 3250
Wire Wire Line
	10650 2600 10550 2600
Wire Wire Line
	10550 2500 10650 2500
Wire Wire Line
	10550 1850 10650 1850
Wire Wire Line
	10650 1750 10550 1750
Wire Wire Line
	10550 1100 10650 1100
Wire Wire Line
	10650 1000 10550 1000
Wire Wire Line
	7400 900  8900 900 
Wire Wire Line
	7400 1000 8900 1000
Wire Wire Line
	7400 1100 8350 1100
Wire Wire Line
	8350 1100 8350 1650
Wire Wire Line
	8350 1650 8900 1650
Wire Wire Line
	8900 1750 8250 1750
Wire Wire Line
	8250 1750 8250 1200
Wire Wire Line
	8250 1200 7400 1200
Wire Wire Line
	7400 1300 8150 1300
Wire Wire Line
	8150 1300 8150 2400
Wire Wire Line
	8150 2400 8900 2400
Wire Wire Line
	8900 2500 8050 2500
Wire Wire Line
	8050 2500 8050 1400
Wire Wire Line
	8050 1400 7400 1400
Wire Wire Line
	3000 900  3450 900 
Wire Wire Line
	3450 1000 2950 1000
Wire Wire Line
	3450 1200 2900 1200
Wire Wire Line
	2900 1300 3450 1300
Wire Wire Line
	2900 1450 3450 1450
Wire Wire Line
	3450 1550 2900 1550
Wire Wire Line
	8150 3350 8900 3350
Wire Wire Line
	8550 3450 8900 3450
Wire Wire Line
	8900 3250 8300 3250
Wire Wire Line
	8300 3150 8900 3150
Wire Wire Line
	4550 3900 4850 3900
Wire Wire Line
	6200 4200 6650 4200
Wire Wire Line
	4500 3700 4850 3700
Wire Wire Line
	4400 4900 4850 4900
Wire Wire Line
	4850 5000 4400 5000
Wire Wire Line
	6800 4800 6200 4800
Wire Wire Line
	6200 4900 6800 4900
Wire Wire Line
	6800 4600 6200 4600
Wire Wire Line
	6200 4500 6850 4500
Wire Wire Line
	5150 1450 5150 1400
Wire Wire Line
	5150 1400 5350 1400
Wire Wire Line
	5350 1500 5200 1500
Wire Wire Line
	5200 1500 5200 1550
Wire Wire Line
	5200 1550 4950 1550
$Comp
L GND #PWR08
U 1 1 58C6A00C
P 5300 1300
F 0 "#PWR08" H 5300 1050 50  0001 C CNN
F 1 "GND" H 5300 1150 50  0000 C CNN
F 2 "" H 5300 1300 50  0000 C CNN
F 3 "" H 5300 1300 50  0000 C CNN
	1    5300 1300
	0    1    1    0   
$EndComp
Wire Wire Line
	5350 1300 5300 1300
Wire Wire Line
	4950 1300 5050 1300
Wire Wire Line
	5050 1300 5050 1200
Wire Wire Line
	5050 1200 5350 1200
Wire Wire Line
	5350 1100 4950 1100
Wire Wire Line
	4950 1100 4950 1200
$Comp
L CONN_01X05 P1
U 1 1 58C6A512
P 7850 5400
F 0 "P1" H 7850 5700 50  0000 C CNN
F 1 "CONN_01X05" V 7950 5400 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x05_Pitch2.54mm" H 7850 5400 50  0001 C CNN
F 3 "" H 7850 5400 50  0000 C CNN
	1    7850 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 5000 6800 5000
Wire Wire Line
	6800 5100 6200 5100
Text Label 6300 5000 0    60   ~ 0
SCL
Text Label 6300 5100 0    60   ~ 0
SDA
Text Label 7350 5600 0    60   ~ 0
SDA
Text Label 7350 5500 0    60   ~ 0
SCL
NoConn ~ 7650 5400
$Comp
L +3V3 #PWR09
U 1 1 58C6A9D6
P 7400 5300
F 0 "#PWR09" H 7400 5150 50  0001 C CNN
F 1 "+3V3" H 7400 5440 50  0000 C CNN
F 2 "" H 7400 5300 50  0000 C CNN
F 3 "" H 7400 5300 50  0000 C CNN
	1    7400 5300
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR010
U 1 1 58C6AA02
P 7400 5100
F 0 "#PWR010" H 7400 4850 50  0001 C CNN
F 1 "GND" H 7400 4950 50  0000 C CNN
F 2 "" H 7400 5100 50  0000 C CNN
F 3 "" H 7400 5100 50  0000 C CNN
	1    7400 5100
	-1   0    0    1   
$EndComp
Wire Wire Line
	7650 5200 7400 5200
Wire Wire Line
	7400 5200 7400 5100
Wire Wire Line
	7400 5300 7650 5300
Wire Wire Line
	7650 5500 7350 5500
Wire Wire Line
	7350 5600 7650 5600
$Comp
L CONN_01X05 P6
U 1 1 58C6AE52
P 8850 5400
F 0 "P6" H 8850 5700 50  0000 C CNN
F 1 "CONN_01X05" V 8950 5400 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x05_Pitch2.54mm" H 8850 5400 50  0001 C CNN
F 3 "" H 8850 5400 50  0000 C CNN
	1    8850 5400
	1    0    0    -1  
$EndComp
Text Label 8350 5600 0    60   ~ 0
SDA
Text Label 8350 5500 0    60   ~ 0
SCL
NoConn ~ 8650 5400
$Comp
L +3V3 #PWR011
U 1 1 58C6AE5B
P 8400 5300
F 0 "#PWR011" H 8400 5150 50  0001 C CNN
F 1 "+3V3" H 8400 5440 50  0000 C CNN
F 2 "" H 8400 5300 50  0000 C CNN
F 3 "" H 8400 5300 50  0000 C CNN
	1    8400 5300
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR012
U 1 1 58C6AE61
P 8400 5100
F 0 "#PWR012" H 8400 4850 50  0001 C CNN
F 1 "GND" H 8400 4950 50  0000 C CNN
F 2 "" H 8400 5100 50  0000 C CNN
F 3 "" H 8400 5100 50  0000 C CNN
	1    8400 5100
	-1   0    0    1   
$EndComp
Wire Wire Line
	8650 5200 8400 5200
Wire Wire Line
	8400 5200 8400 5100
Wire Wire Line
	8400 5300 8650 5300
Wire Wire Line
	8650 5500 8350 5500
Wire Wire Line
	8350 5600 8650 5600
$Comp
L R R1
U 1 1 58C6B1BA
P 3750 4000
F 0 "R1" V 3830 4000 50  0000 C CNN
F 1 "R" V 3750 4000 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 3680 4000 50  0001 C CNN
F 3 "" H 3750 4000 50  0000 C CNN
	1    3750 4000
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 58C6B243
P 3750 4200
F 0 "R2" V 3830 4200 50  0000 C CNN
F 1 "R" V 3750 4200 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 3680 4200 50  0001 C CNN
F 3 "" H 3750 4200 50  0000 C CNN
	1    3750 4200
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 58C6B284
P 3750 4400
F 0 "R3" V 3830 4400 50  0000 C CNN
F 1 "R" V 3750 4400 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 3680 4400 50  0001 C CNN
F 3 "" H 3750 4400 50  0000 C CNN
	1    3750 4400
	0    1    1    0   
$EndComp
$Comp
L LED D1
U 1 1 58C6B2BA
P 3300 4000
F 0 "D1" H 3300 4100 50  0000 C CNN
F 1 "LED" H 3300 3900 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 3300 4000 50  0001 C CNN
F 3 "" H 3300 4000 50  0000 C CNN
	1    3300 4000
	1    0    0    -1  
$EndComp
$Comp
L LED D2
U 1 1 58C6B35B
P 3300 4200
F 0 "D2" H 3300 4300 50  0000 C CNN
F 1 "LED" H 3300 4100 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 3300 4200 50  0001 C CNN
F 3 "" H 3300 4200 50  0000 C CNN
	1    3300 4200
	1    0    0    -1  
$EndComp
$Comp
L LED D3
U 1 1 58C6B3A5
P 3300 4400
F 0 "D3" H 3300 4500 50  0000 C CNN
F 1 "LED" H 3300 4300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 3300 4400 50  0001 C CNN
F 3 "" H 3300 4400 50  0000 C CNN
	1    3300 4400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 58C6B574
P 2950 4550
F 0 "#PWR013" H 2950 4300 50  0001 C CNN
F 1 "GND" H 2950 4400 50  0000 C CNN
F 2 "" H 2950 4550 50  0000 C CNN
F 3 "" H 2950 4550 50  0000 C CNN
	1    2950 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 4400 2950 4400
Wire Wire Line
	2950 4000 2950 4550
Wire Wire Line
	3150 4200 2950 4200
Connection ~ 2950 4400
Wire Wire Line
	3150 4000 2950 4000
Connection ~ 2950 4200
Wire Wire Line
	3450 4000 3600 4000
Wire Wire Line
	3600 4200 3450 4200
Wire Wire Line
	3450 4400 3600 4400
Wire Wire Line
	4850 4000 3900 4000
Wire Wire Line
	3900 4200 3950 4200
Wire Wire Line
	3950 4200 3950 4100
Wire Wire Line
	3950 4100 4850 4100
Wire Wire Line
	4850 4200 4050 4200
Wire Wire Line
	4050 4200 4050 4400
Wire Wire Line
	4050 4400 3900 4400
$EndSCHEMATC
