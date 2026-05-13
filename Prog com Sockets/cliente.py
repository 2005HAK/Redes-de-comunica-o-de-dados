import socket
import struct

HOST = '172.20.10.2'
PORT = 8081

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print("Conectado ao servidor QAP. Digite 'sair' para encerrar.")
    
    while True:
        pergunta = input("Sua Pergunta: ")
        if pergunta.lower() == 'sair':
            break
            
        # 1. Prepara e envia a requisição
        payload_bytes = pergunta.encode('utf-8')
        # Version 1, Type 1 (Request), Length
        header = struct.pack('!BBH', 1, 1, len(payload_bytes))
        s.sendall(header + payload_bytes)
        
        # 2. Recebe a resposta
        res_header = s.recv(4)
        if not res_header:
            break
            
        version, msg_type, payload_length = struct.unpack('!BBH', res_header)
        
        if msg_type == 2: # Response
            resposta = s.recv(payload_length).decode('utf-8')
            print(f"Resposta do Servidor: {resposta}\n")