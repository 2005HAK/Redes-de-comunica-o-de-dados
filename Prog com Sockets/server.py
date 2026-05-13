import socket
import struct

HOST = '172.20.10.2'
PORT = 8081

# Base de conhecimento simples
KNOWLEDGE_BASE = {
    "qual e a capital do brasil?": "Brasília",
    "quanto e 2+2?": "4",
    "qual o sentido da vida?": "42"
}

def handle_client(conn, addr):
    print(f"Conectado a {addr}")
    try:
        while True:
            # 1. Lê os 4 bytes do cabeçalho
            header = conn.recv(4)
            if not header:
                break
            
            # Desempacota o cabeçalho: B (unsigned char, 1 byte), B (1 byte), H (unsigned short, 2 bytes)
            version, msg_type, payload_length = struct.unpack('!BBH', header)
            
            if msg_type == 1: # Request
                # 2. Lê o payload com base no tamanho informado
                payload = conn.recv(payload_length).decode('utf-8')
                print(f"Pergunta recebida: {payload}")
                
                # 3. Processa a resposta
                pergunta_formatada = payload.lower().strip()
                resposta = KNOWLEDGE_BASE.get(pergunta_formatada, "Desculpe, não sei a resposta.")
                
                # 4. Empacota e envia a resposta
                payload_bytes = resposta.encode('utf-8')
                # Version 1, Type 2 (Response), Length
                response_header = struct.pack('!BBH', 1, 2, len(payload_bytes))
                conn.sendall(response_header + payload_bytes)
                
    except Exception as e:
        print(f"Erro: {e}")
    finally:
        conn.close()

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    print(f"Servidor QAP rodando na porta {PORT}...")
    while True:
        conn, addr = s.accept()
        handle_client(conn, addr)