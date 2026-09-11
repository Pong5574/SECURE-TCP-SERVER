# SECURE-TCP-SERVER
โปรเจคนี้เป็นการสร้าง tcp server เพื่อแสดงให้เห็นถึงความสามารถของผมเพื่อเอาไปใส่ใน portfilo ในการเข้า ม.เกษตร :D

# server.cpp

WSADATA win_sock_api;
^
คือตัวแปรที่เอาไว้เริ่มการเชื่อมต่อกับ windows api ในการดึง winsock2 มาใช้งาน

int start_up = WSAStartup(MAKEWODE(2, 2), &win_sock_api);
^
คือการสร้างตัวแปรประเภท integer เพื่อเก็บข้อมูล WSAStartup() คือ func ในการเริ่มการดึง libary ต่างๆมาข้างในมี MAKEWORD() คือการเลือก version ของ winsock จากตัวอย่างได้เลือกเป็น version 2.2 และอีกส่วนนึงต้องการที่อยู่ของตัวแปรที่เก็บการเชื่อมกับ windows socket api ไว้และสามารถเอาค่าที่ WSAStartup() return ออกมาเอาไปเช็คได้ว่า success หรือมั้ย

SOCKET srv_sock = SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
^
เป็นการสร้างตัวแปรเพื่อเก็บ socket ที่ใช้ในการเชื่อมต่อเริ่มด้วย func socket() ประกอบไปด้วย 
* AF_INET คือการเลือกใช้ IPv4
* SOCK_STREAM คือการเลือกใช้การสื่อสารแบบ stream
* IPPROTO_TCP คือการเลือก protocal TCP ในการรับส่งข้อมูล
และ srv_sock สามารถเอาไปเช็คได้ว่า socket ถูกสร้างสำเร็จมั้ยด้วยการเช็คด้วย INVALID_SOCKET เช่น if (srv_sock == INVALID_SOCKET) {return 1;} เป็นต้นและเมื่อจบโปรแกรมหรือไม่ได้ใช้งานแล้วให้ปิดการใช้งานด้วย closesocket(srv_sock)

sockaddr_in srv_addr{};
^
คือการสร้างตัวแปรชื่อ srv_addr หรือ server address ให้เป็นตัวแปรประเภท structer เอาไว้เก็บข้อมูล address ของตัว server หลักๆ ประกอบไปด้วย ip กับ port และ ประเภท ip ที่เลือกใช้

bind(socket, address, address size)
^
เป็นการผูก socket เข้ากับตัว address ของ server เพื่อให้สามารถเปิดการเชื่อมต่อได้

listen(socket, max client)
^
เป็นขั้นตอนการเริ่มรอการเชื่อมของ client ประกอบไปด้วย socket ของ server และจำนวน client ที่ต้องการรับสูงสุดถ้าใช้เป็น SOMAXCONN คือการปรับเป็นสูงสุดที่ OS รองรับ

accept(socket, client address, address size)
^
เป็นขั้นตอนในการรับ client เชื่อมต่อเข้ามาใน server

และมีการจำลองหรือเรียนแบบการส่งแบบ 3 handshake ระหว่าง client

# client.cpp

ส้วนประกอบหลักจะคล้ายๆกับตัว server.cpp แต่จะไม่มี bind() และ listen() และจะเพิ่ม connect() เข้ามาเพิ่ม

connect(socket, server address, address size)
^
เป็นการเชื่อมต่อไปยังตัว server และตัว server ก้จะรับการเชิ่อมต่อด้วย accept()

มีการรับค่าเป้น username กับ password แล้วทำการส่งไปให้ server ตรวจเช็ค

# database

ผมใช้ db ของ sqlite

open_db()
^
เป็น func เปิด database ที่ข้างในเป็นการ รับตัวแปรที่เก็บ db ไว้แล้วนำว่าสั่งเปืดด้วย sqlite3_open() ประกอบไปด้วยชื่อไฟล์ที่ต้องการเปิดหรือสร้างถ้าไม่มีและเมื่อไม่ใช้ db แล้วต้องปิดด้วน sqlite3_close(db)

create_table()
^
เป็นการสร้าง table เก็บข้อมูลด้วยคำสั่ง sql คำสั่งที่ใช้คือ CREATE TABLE IF NOT EXISTS User (Username TEXT UNIQUE NOT NULL, Password TEXT NOT NULL);

CREATE TABLE + User คือสั่งสร้าง table ชื่อ User
IF NOT EXISTS คือถ้า table ยังไม่ถูกสร้างให้สร้างและถ้ามีการสร้างแล้วก็ไม่ต้องสร้าง
table ประกอบไปด้วย Username TEXT UNIQUE NOT NULL, Password TEXT NOT NULL
  * Username, Password คือสมาชิกของ table
  * TEXT คือประเภทเป็น string หรือตัวอักษร
  * NOT NULL คือห้ามเป็นค่าว่าง
  * UNIQUE คือห้ามมีการซ่ำเกิดขึ้น
