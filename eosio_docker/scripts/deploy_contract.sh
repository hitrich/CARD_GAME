#!/usr/bin/env bash
set -o errexit

# set PATH
PATH="$PATH:/opt/eosio/bin"

CONTRACTSPATH="$( pwd -P )/contracts"

# создать новый каталог для скомпилированных контрактных файлов
mkdir -p ./compiled_contracts
mkdir -p ./compiled_contracts/$1

COMPILEDCONTRACTSFOLDER="compiled_contracts"
COMPILEDCONTRACTSPATH="$( pwd -P )/$COMPILEDCONTRACTSFOLDER"

# разблокировать кошелек, игнорировать ошибку, если уже разблокирован
if [ ! -z $3 ]; then cleos wallet unlock -n $3 --password $4 || true; fi

# компилировать умный контракт в файлы wasm и abi, используя EOSIO.CDT (Contract Development Toolkit)
# https://github.com/EOSIO/eosio.cdt
(
  eosio-cpp -abigen "$CONTRACTSPATH/$1/$1.cpp" -o "$COMPILEDCONTRACTSPATH/$1/$1.wasm" --contract "$1"
) &&

# установить (развернуть) скомпилированный контракт в блокчейн
cleos set contract $2 "$COMPILEDCONTRACTSFOLDER/$1" --permission $2
