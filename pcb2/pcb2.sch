EESchema Schematic File Version 4
EELAYER 30 0
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
L MCU_Microchip_ATmega:ATmega328P-AU U?
U 1 1 5EFE81A4
P 3500 3000
F 0 "U?" H 2900 1750 50  0000 C CNN
F 1 "ATmega328P-AU" H 2650 1650 50  0000 C CNN
F 2 "Package_QFP:TQFP-32_7x7mm_P0.8mm" H 3500 3000 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 3500 3000 50  0001 C CNN
	1    3500 3000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EFF02CC
P 3500 4500
F 0 "#PWR?" H 3500 4250 50  0001 C CNN
F 1 "GND" H 3505 4327 50  0000 C CNN
F 2 "" H 3500 4500 50  0001 C CNN
F 3 "" H 3500 4500 50  0001 C CNN
	1    3500 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 1500 3500 1500
$Comp
L power:+5V #PWR?
U 1 1 5EFF1169
P 3500 1500
F 0 "#PWR?" H 3500 1350 50  0001 C CNN
F 1 "+5V" H 3515 1673 50  0000 C CNN
F 2 "" H 3500 1500 50  0001 C CNN
F 3 "" H 3500 1500 50  0001 C CNN
	1    3500 1500
	1    0    0    -1  
$EndComp
Connection ~ 3500 1500
$Comp
L Device:C_Small C?
U 1 1 5EFF2782
P 3050 1150
F 0 "C?" H 3142 1196 50  0000 L CNN
F 1 ".1u" H 3142 1105 50  0000 L CNN
F 2 "" H 3050 1150 50  0001 C CNN
F 3 "~" H 3050 1150 50  0001 C CNN
	1    3050 1150
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1_Small C?
U 1 1 5EFF474A
P 3350 1150
F 0 "C?" H 3259 1104 50  0000 R CNN
F 1 "4.7uF" H 3259 1195 50  0000 R CNN
F 2 "" H 3350 1150 50  0001 C CNN
F 3 "~" H 3350 1150 50  0001 C CNN
	1    3350 1150
	-1   0    0    1   
$EndComp
Wire Wire Line
	3500 1500 3350 1500
Wire Wire Line
	3350 1500 3350 1250
Wire Wire Line
	3350 1250 3050 1250
Connection ~ 3350 1250
Wire Wire Line
	3050 1050 3350 1050
$EndSCHEMATC
