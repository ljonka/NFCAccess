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
	<td>8 (SDA)</td>
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
	<td>7 (SCK)</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>A4</td>
	<td>5 (MISO)</td>
	<td></td>
	<td></td>
	<td></td>
	<td></td>
<tr>
<tr>
	<td>A5</td>
	<td>6 (MOSI)</td>
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
	<td>3 (IN)</td>
	<td></td>
<tr>
<tr>
	<td>D1</td>
	<td></td>
	<td></td>
	<td>2</td>
	<td></td>
	<td>2</td>
<tr>
<tr>
	<td>D2</td>
	<td>2 (RST)</td>
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
	<td>1 (3,3V)</td>
	<td></td>
	<td></td>
	<td></td>
	<td>1</td>
<tr>
<tr>
	<td>GND</td>
	<td>3 (GND)</td>
	<td>1 (GND)</td>
	<td>1</td>
	<td>2 (GND)</td>
	<td></td>
<tr>
<tr>
	<td>VIN(5V)</td>
	<td></td>
	<td>2 (VCC)</td>
	<td></td>
	<td>1 (VCC)</td>
	<td></td>
<tr>

</table>

<h3>Hardware list</h3>
<ul>
	<li>Particle Photon STM32F2</li>
	<li>RFID-RF522 MFRC522</li>
	<li>MicroSD Card Adapter Catalex</li>
	<li>Taster</li>	
	<li>Relais Module SRD-05VDC-SL-C</li>	
	<li>1,5MOhm Resistor</li>
	
</ul>

<h2>Managing users</h2>
Users are created on files with name equal to rfid uid, hold your card on the reader and look in log file on sd card for available uid's. To allow acces for specific uid create a simple text file with the following content (its json): 

Example - allow uid "_123_123_123_123" access each day, Filename: "_123_123_123_123" in SDCard's Root Directory
<pre>
{"u":"_123_123_123_123", "t":[1,1,1,1,1,1,1]}
</pre>

The "t" Param stands for "Access Time", 1 = allow, 0 = permit. One Value for each weekday, starting on Sunday.

<h3>Insert user from cloud</h3>
Send the json over cloud with the following command:
<pre>
particle call your-device-id updateKey '{"u":"_123_123_123_123", "t":[1,1,1,1,1,1,1]}'
</pre>

This should create a file naming "_123_123_123_123" on SDCard and set the rights as requested.



