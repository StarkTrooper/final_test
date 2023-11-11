
<br />
<p align="center">
  <h1 align="center">Final test</h1>

  <p align="center">
    Main project for stress detection wristband
  </p>
</p>

<br>



## Abstract

This is the C version of the Mood Detection project.

The project focuses on the first version of the final code to be implemented in the stress detection wristband prior to the development of the mobile application.

This algorithm aims to be easy and cheap to implement in order to avoid the use of machine learnig libraries.

<br>


## Important Notes

* The Heart Rate HR() calculation involves performing a quotient, a costly operation, so its calculation was removed and only the peak-to-peak distance was used, if the distance is large the heart rate is high, and if the distance is short, the heart rate is high. At a rate of 25 samples per second, the normal peak-to-peak distance is about 20. If this is above 23, the heart rate is low and below 18, the heart rate is high. These parameters can be adjusted.

* With the above, the Heart Rate is no longer stored, only one of three options is displayed:
 - 0: low heart rate
 - 1: normal heart rate
 - 2: high heart rate

* To connect to the stress algorithm, it receives this value from the HR() function. When obtaining this value.

* In order to take into account the movement of all axes, it was thought to take an average, but the computational cost is high: Take the sum or take the maximums. The attached version implements the obtaining of the maximums.

* In the actual code, global variables should be avoided, and therefore every effort should be made to call functions within the main() of the main() file.

* The stress algorithm prints warning messages to corroborate operation, however these will be replaced by the output of a number from 1 to 3 or another signal indicating the output response of the wristband, which will be used to turn the vibration motor and to turn on any LEDs if necessary.

* Previously only one heart rate was sent, now the heart rate is calculated with the input data and the last one is sent as a response.

* It is proposed to give time windows so as not to make calculations all the time. Or, to make the calculation when some kind of external signal is received.

* The implementation of if else was changed to switch case, since it is less expensive.

* Now the functions called from the "bluetooth" and "HR_Strss_01" files can be used on main.c

* The file HR_Strss_01.c is not called in main.c but can be extracted and compiled separately to see how the prediction algorithm works on its own.

* This is a private repository  until the  project is  finished.

<br>

### Software used

*  C
*  Language: C
*  IDE: Visual Studio Code
*  Framework: nRF Connect 
*  Main support: Zephyr project

---
**Contact - velramedu@gmail.com**