<h1>Welcome to the NFCAccess Tool</h1>

Functional description: Check User Permission on daily basis with Particle (Photon) on RFID Module MFRC522 against Files on SD-Card, saved in JSON Format.

<h2>Prerequisite to compile for Particle</h2>
<pre>
npm install -g particle-cli
particle login
</pre>

<h2>Installation</h2>
<pre>
git clone https://github.com/ljonka/NFCAccess.git
cd NFCAccess
git submodule update --init --recursive
</pre>

<h2>Compile or flash for particle photon</h2>

<h3>Compile and download bin</h3>
<pre>particle compile photon .</pre>

<h3>Compile and flash</h3> 
<pre>particle flash <device id> .</pre>

<h2>Wiring</h2>

<table>
<tr>
	<td>Particle Photon</td>
	<td>MFRC522</td>
	<td>MicroSD Card Adapter</td>
	<td>Taster</td>
	<td>Relais Module</td>
	<td>Resistor</td>
<tr>
<tr>
	<td>A0</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>A1</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>A2</td>
	<td></td>
	<td>6 (CS)</td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>A3</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>A4</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>A5</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>D0</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>D1</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>D2</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>D3</td>
	<td></td>
	<td>5 (SCK)</td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>D4</td>
	<td></td>
	<td>3 (MISO)</td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>D5</td>
	<td></td>
	<td>4 (MOSI)</td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>3,3V</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>GND</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>VIN(5V)</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>

</table>




