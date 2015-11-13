car-infotainment
==========

A work-in-progress arduino car infotainment display. It currently has two temperature sensors and an AltIMU board which feeds the Arduino with an altitude and compass reading. Currently, the measuring units are the following: temperature in degrees Celsius, compass heading in degrees and altitude reading in meters. <br/>
I've renamed the AltIMU sensors' libraries, so their names are more descriptive (<a href="/pololu/l3g-arduino">L3G</a>, <a href="/pololu/lps-arduino">LPS</a> and <a href="/pololu/minimu-9-ahrs-arduino">LSM303</a>).

The following modules are used:
<ul>
	<li>1x Arduino UNO r3 board </li>
	<li>2x DS18S20 water proof temperature sensors </li>
	<li>1x AltIMU board </li>
	<li>1x 16x2 SerLCD display </li>
</ul>
<br/>
If setup correctly, the screen should display the information in the following way (without the quotes):
<ul>
	<li><pre>"I: 25.0 O: 35.0 "</pre> (input and output temperature, respectively)</li>
	<li><pre>"N  358.0o 1000m^"</pre> (compass direction - N, NE, E, SE, S, SW, W, NW; compass heading in degrees; altitude in meters; and display refresh animation)</li>
</ul>
