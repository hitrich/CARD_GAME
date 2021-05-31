# Dragon Card
Dragon Card - это карточная игра для одного игрока, которая разворачивается в фантазийном мире, где вы, игрок, можете использовать силу изумрудов  Дерева, Огня и Воды, чтобы выиграть своего противника! Игра ведется против искусственного интеллекта на основе интеллектуального контракта(smart contracts) и разворачивается на живой цепочке блоков блокчейн.
 
**- *Account*: `player1`**
**- *Private Key*: `5KFyaxQW8L6uXFB6wSgC44EsAbzC7ideyhhQ68tiYfdKQp69xKo`**

#Начальная настройка
Убедитесь, что Docker и Node.js установлены

* Установите Docker: https://docs.docker.com/docker-for-mac/install/
* Установите Node.js: https://nodejs.org/en/

DApp и eosio займут порты 3000, 8888 и 9876. Убедитесь, что на этих портах больше ничего не запущено.
```./first_time_setup.sh```
Выполнение приведенного выше сценария оболочки проверяет, установлены ли docker и node.js. Затем он создает образ докера eosio-notechain, если он никогда не создавался ранее (который содержит полную версию блокчейна eosio), удаляет все предыдущие экземпляры этого контейнера док-станции и устанавливает пакеты узлов для react app (внешнего интерфейса).

Инициализировать и запустить блокчейн и DApp
После инициализации требуются два окна терминала, оба открываются в каталоге хранилища.

Первое оконечное окно для процесса блокчейна.
Второе окно терминала предназначено для внешнего интерфейса.

#Для первого (блокчейн) терминального окна 
```./start_eosio_docker.sh```




#Для второго (внешнего) окна терминала.
```./start_frontend.sh```
откроет сеанс браузера, соединяющийся с http: // localhost: 3000 /, показывая react app. Вы можете попробовать добавить или удалить заметки, используя одну из предварительно созданных учетных записей с ее ключом на веб-сайте. React app будет взаимодействовать со смарт-контрактом, выполняя транзакции, которые записываются в блокчейн, который хранит данные заметок в многоиндексной таблице смарт-контракта, выполняемой на локальных узлах.

Остановка блокчейна или DApp
В первом (блокчейн) окне терминала нажмите Ctrl + C на клавиатуре, журнал прекратит печать. И затем выполните:

Остановка докера eosio_notechain_container
Это действие займет несколько секунд. Блокчейн будет остановлен.
Остановка DApp
Во втором (внешнем) окне терминала нажмите Ctrl + C на клавиатуре. React app с внешним интерфейсом будет остановлено.

Перезапуск блокчейна или DApp
В первом (блокчейн) окне терминала выполните эту команду:
```./start_eosio_docker.sh```
Блокчейн будет автоматически возобновлен, и журнал будет выведен на терминал.
Перезапуск DApp
Во втором (внешнем) терминальном окне вы можете перезапустить приложение внешнего интерфейса, выполнив:
```./start_frontend.sh```
Сбросить данные блокчейна
Сначала вам нужно остановить цепочку блоков (как указано выше), а затем выполнить:
```./first_time_setup.sh```
При этом удаляются все данные в цепочке блоков, включая учетные записи, развернутые интеллектуальные контракты и т. Д. Счетчик блоков будет сброшен при повторном запуске цепочки блоков.


#Использование Docker
Docker используется для обертывания программного обеспечения eosio внутри и запуска контейнера (экземпляра) из изображения (eosio-notechain). Чтобы работать с блокчейном напрямую, запустив сценарии или используя командную строку cleos, вам нужно перейти в контейнерный bash.

#Вход в контейнер Bash:
docker exec -it eosio_notechain_container bash
Проект имеет рабочий каталог контейнера в / opt / eosio / bin /, вы можете запустить команду cleos в этом каталоге напрямую. Для документации ключей: https://developers.eos.io/eosio-nodeos/docs/cleos-overview

Вы также можете посмотреть в скриптах init_blockchain.sh или deploy_contract.sh примеры командной строки cleos.

Чтобы выйти изнутри контейнера:
exit

Внутри док-контейнера

```./scripts/deploy_contract.sh notechain notechainacc notechainwal $ (cat notechain_wallet_password.txt)```
После запуска этого сценария измененный смарт-контракт будет развернут на блокчейне.


#Внешний интерфейс:
Команды Docker
Если вы более знакомы с Docker, вы можете использовать команды Docker ниже, чтобы лучше контролировать всю среду. Ниже приведены объяснения каждой из команд:

Выполните следующую команду в / eosio_docker:

Запустите контейнер из образа eosio-notechain, подключив контракты / сценарии к контейнеру, запустив сценарий init_blockchain.sh в качестве процесса. Сценарий init_blockchain.sh запускает локальный узел блокчейна и инициализирует кошельки / контракт / данные.

```docker run --rm --name eosio_notechain_container \
-p 8888:8888 -p 9876:9876 \
--mount type=bind,src="$(pwd)"/contracts,dst=/opt/eosio/bin/contracts \
--mount type=bind,src="$(pwd)"/scripts,dst=/opt/eosio/bin/scripts \
--mount type=bind,src="$(pwd)"/data,dst=/mnt/dev/data \
-w "/opt/eosio/bin/" eosio-notechain:eos1.8.6-cdt1.6.2 /bin/bash -c "./scripts/init_blockchain.sh" ```

Выведите и следуйте журналам консоли докера:

Логи докера eosio_notechain_container --follow
Удалить контейнер (удалит все кошельки / контракты / данные),  если вы хотите повторно запустить весь DApp.
docker rm -f eosio_notechain_container
Остановите контейнер 
docker stop eosio_notechain_container
