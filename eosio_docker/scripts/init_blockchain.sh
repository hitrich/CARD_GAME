#!/usr/bin/env bash
set -o errexit

echo "=== setup blockchain accounts and smart contract ==="

# set PATH
PATH="$PATH:/opt/eosio/bin:/opt/eosio/bin/scripts"

set -m

# start nodeos (локальный узел блокчейна)
nodeos -e -p eosio -d /mnt/dev/data \
  --config-dir /mnt/dev/config \
  --http-validate-host=false \
  --plugin eosio::producer_plugin \
  --plugin eosio::history_plugin \
  --plugin eosio::chain_api_plugin \
  --plugin eosio::history_api_plugin \
  --plugin eosio::http_plugin \
  --http-server-address=0.0.0.0:8888 \
  --access-control-allow-origin=* \
  --contracts-console \
  --verbose-http-errors &
sleep 1s
until curl localhost:8888/v1/chain/get_info
do
  sleep 1s
done

sleep 2s
echo "=== setup wallet: eosiomain ==="
# Первый импорт ключей для системной учетной записи eosio
cleos wallet create -n eosiomain --to-console | tail -1 | sed -e 's/^"//' -e 's/"$//' > eosiomain_wallet_password.txt
cleos wallet import -n eosiomain --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

echo "=== setup wallet: cardgamewal ==="
# ключ для учетной записи eosio и экспорт сгенерированного пароля в файл для разблокировки кошелька позже
cleos wallet create -n cardgamewal --to-console | tail -1 | sed -e 's/^"//' -e 's/"$//' > cardgame_wallet_password.txt
# Ключ владельца для карточного кошелька
cleos wallet import -n cardgamewal --private-key 5JpWT4ehouB2FF9aCfdfnZ5AwbQbTtHBAwebRXt94FmjyhXwL4K
# Активный ключ для карточного кошелька
cleos wallet import -n cardgamewal --private-key 5JD9AGTuTeD5BXZwGQ5AtwBqHK21aHmYnTetHgk1B3pjj7krT8N

# создание аккаунтов для cardgameacc с открытыми ключами кошелька
cleos create account eosio cardgameacc EOS6PUh9rs7eddJNzqgqDx1QrspSHLRxLMcRdwHZZRL4tpbtvia5B EOS8BCgapgYA2L4LJfCzekzeSr3rzgSTUXRXwNi8bNRoz31D14en9

echo "=== deploy smart contract ==="
# $1 умное имя контракта
# $2 имя владельца смарт-контракта
# $3 кошелек для разблокировки аккаунта
# $4 пароль для разблокировки кошелька
deploy_contract.sh cardgame cardgameacc cardgamewal $(cat cardgame_wallet_password.txt)

echo "=== create user accounts ==="
# скрипт для создания данных в блокчейн
create_accounts.sh

echo "=== end of setup blockchain accounts and smart contract ==="
# создание файла, чтобы указать, что блокчейн был инициализирован
touch "/mnt/dev/data/initialized"

#  фоновое задание nodeos на передний план для запуска докера
fg %1
