#!/usr/bin/env bash
set -o errexit

# этот файл используется для продолжения остановленной блокчейн

echo "=== start blockchain with  ==="

# set PATH
PATH="$PATH:/opt/eosio/bin"

set -m

# start nodejs (локальный узел блокчейна)
# Запуска его в фоновом режиме, чтобы запуск докера продолжался
nodeos -e -p eosio -d /mnt/dev/data \
  --config-dir /mnt/dev/config \
  --hard-replay \
  --http-validate-host=false \
  --plugin eosio::producer_plugin \
  --plugin eosio::history_plugin \
  --plugin eosio::chain_api_plugin \
  --plugin eosio::history_api_plugin \
  --plugin eosio::http_plugin \
  --http-server-address=0.0.0.0:8888 \
  --access-control-allow-origin=* \
  --contracts-console \
  --verbose-http-errors

# опция `--hard-replay` необходима
# потому что сигнал остановки докера не передается процессу nodeos напрямую
# когда мы запускаем init_blockchain.sh как PID 1.
