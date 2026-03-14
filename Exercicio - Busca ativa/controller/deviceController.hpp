#ifndef DEVICECONTROLLER_HPP
#define DEVICECONTROLLER_HPP

#include <iostream>
#include <vector>
#include <sqlite3.h>
#include "device.hpp"

const std::string DB_FILE = "../data/devices.db";

class DeviceController {
	private:
		std::vector<Device> devices;

		/**
		 * @brief Callback para carregar os dispositivos do banco de dados.
		 * 
		 * Este método é chamado para cada linha retornada pela consulta SQL que recupera os dispositivos do banco de dados. Ele processa os dados da linha, cria um objeto `Device` correspondente e o adiciona à lista de dispositivos gerenciados.
		 * 
		 * @param data Ponteiro para os dados do controlador, usado para acessar a lista de dispositivos.
		 * @param argc O número de colunas retornadas pela consulta SQL.
		 * @param argv Um array de strings contendo os valores das colunas para a linha atual.
		 * @param azColName Um array de strings contendo os nomes das colunas retornadas pela consulta SQL.
		 * @return Retorna 0 para indicar que o processamento foi bem-sucedido.
		 */
		static int loadCallback(void* data, int argc, char** argv, char** azColName);

	public:

		/**
		 * @brief Adiciona um dispositivo à lista de dispositivos gerenciados.
		 * 
		 * Este método cria um novo objeto `Device` com as informações fornecidas e o adiciona à lista de dispositivos. O dispositivo é inicialmente inativo e com intervalo de captura definido como 0.
		 * O metodo valida os parâmetros de entrada para garantir que o nome, IP e MAC sejam fornecidos corretamente. Se algum dos parâmetros for inválido, o método pode lançar uma exceção ou retornar um erro indicando o problema.
		 * 
		 * @param name O nome do dispositivo.
		 * @param ip O endereço IP do dispositivo.
		 * @param mac O endereço MAC do dispositivo.
		 */
		void addDevice(const std::string& name, const std::string& ip, const std::string& mac);

		/**
		 * @brief Remove um dispositivo da lista de dispositivos gerenciados.
		 * 
		 * Este método procura um dispositivo na lista de dispositivos com o nome fornecido e o remove da lista. Se nenhum dispositivo com o nome especificado for encontrado, o método pode lançar uma exceção ou retornar um erro indicando que o dispositivo não existe.
		 * 
		 * @param name O nome do dispositivo a ser removido.
		 */
		void removeDevice(const std::string& name);

		/**
		 * @brief Carrega os dispositivos do banco de dados.
		 * 
		 * Este método estabelece uma conexão com o banco de dados SQLite, executa uma consulta para recuperar os dispositivos armazenados e os adiciona à lista de dispositivos gerenciados. O método utiliza um callback para processar os resultados da consulta e criar objetos `Device` correspondentes.
		 */
		void loadDevicesBD();

		/**
		 * @brief Salva os dispositivos no banco de dados.
		 * 
		 * Este método estabelece uma conexão com o banco de dados SQLite, limpa a tabela de dispositivos e insere os dispositivos atualmente gerenciados na tabela. O método constrói e executa comandos SQL para realizar essas operações, garantindo que o estado atual dos dispositivos seja refletido no banco de dados.
		 */
		void saveDevicesBD();
};

#endif // DEVICECONTROLLER_HPP