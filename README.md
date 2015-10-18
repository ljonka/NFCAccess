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




