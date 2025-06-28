# Telecontrol pentru sistem robotic cu membre articulate

## 1. Configurare Raspberry Pi 5

Pentru a folosi placa Raspberry Pi 5, trebuie să instalați sistemul de operare Raspberry Pi OS pe un card MicroSD. Introduceți cardul în portul MicroSD de pe lateralul plăcii și conectați alimentarea; placa va boota automat în sistemul de operare.

### 1.1. Activare comunicații seriale și SSH

1. Din interfața de configurare (`sudo raspi-config` sau din **Settings → Raspberry Pi Configuration**):
   - În **Interfaces**, activați **Serial** (disable shell over serial, enable hardware serial port).
   - În **Interfaces**, activați **SSH**.  
   
   ![Activare Serial](https://github.com/BogdanM0103/Telecontrol-pentru-sistem-robotic-cu-membre-articulate/blob/detached/20250628_13h13m00s_grim.png)  
   ![Activare SSH](https://github.com/BogdanM0103/Telecontrol-pentru-sistem-robotic-cu-membre-articulate/blob/detached/20250628_13h13m07s_grim.png)

2. Asigurați-vă că Raspberry Pi este conectată la aceeași rețea de internet cu dispozitivul de pe care veți clona repository-ul.

### 1.2. Instalare dependențe

Deschideți un terminal și rulați:

```bash
sudo apt update
sudo apt install cmake build-essential
pip3 install flask
```
## 2. Configurare CLion și Build & Deploy pe Raspberry Pi

Pentru a compila și rula aplicația direct pe Raspberry Pi din CLion, urmați pașii de mai jos.

### 2.1. Configurare Remote Toolchain

1. Mergi la **File → Settings → Build, Execution, Deployment → Toolchains**.  
2. Creează un nou toolchain de tip **Remote Host**:  
   - **Host**: IP-ul RPi (ex. `192.168.1.x`), port 22.  
   - **User**: `pi` și cheia privată SSH (sau parolă).  
   - CLion detectează automat `gcc`, `g++`, `make` și CMake pe Pi.  
3. În secțiunea **Deployment**, setează **Mapping** (ex. `/home/pi/project_sync`) și apoi **Apply**.

### 2.2. Creare profil CMake Remote

1. Accesează **File → Settings → Build, Execution, Deployment → CMake**.  
2. Adaugă un nou profil de build, numit “Remote-RPi”:  
   - **Toolchain**: selectează toolchain-ul Remote creat anterior.  
   - **Build directory**: `cmake-build-remote` (în rădăcina proiectului).
   - **CMake options**: -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DDEPLOY_HOST=192.168.1.x -DDEPLOY_USER=user -DDEPLOY_PATH=/home/user/directory/Hexapod/cmake-build-debug
3. Salvează și selectează profilul “Remote-RPi” pentru build.

### 2.3. Build & Deploy

1. Selectează profilul “Remote-RPi” și execută **Build → Build Project**.  
   - CLion va urca (SFTP) sursele pe Pi și va rula `make` acolo.  
2. Creează o configurație de tip **Remote Application**:  
   - **Executabil**: `/home/pi/directory/cmake-build-remote/<numele_executabilului>`.  
   - **Host**: același Remote Host.  
3. Pe placa Raspberry Pi, în locul unde există fișierul "udp_server.py", rulează în terminal "python3 udp_server.py"
4. Rulează din Clion scriptul "server.py" cu **Run → Run '<config>'**. Binarul va fi copiat și lansat pe Pi, iar adresa webserverului apare în consola locală.

## 3. Clonare și rulare proiect

După configurarea mediului, clonează și compilează repository-ul:

```bash
git clone https://github.com/BogdanM0103/Telecontrol-pentru-sistem-robotic-cu-membre-articulate.git
cd Telecontrol-pentru-sistem-robotic-cu-membre-articulate
mkdir build && cd build
cmake ..
make
